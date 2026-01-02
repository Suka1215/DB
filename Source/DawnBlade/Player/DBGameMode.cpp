// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/DBGameMode.h"
#include "Characters/DBPlayerCharacter.h"
#include "Player/DBPlayerController.h"
#include "Player/DBPlayerState.h"
#include "Player/DBHUD.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogDBServer, Log, All);

ADBGameMode::ADBGameMode()
{
    DefaultPawnClass = ADBPlayerCharacter::StaticClass();
    PlayerControllerClass = ADBPlayerController::StaticClass();
    PlayerStateClass = ADBPlayerState::StaticClass();
    HUDClass = ADBHUD::StaticClass();

    // Enable ticking for batched operations
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    // Tick at a reduced rate - we batch operations
    PrimaryActorTick.TickInterval = 0.05f; // 20 Hz

    UE_LOG(LogDBServer, Log, TEXT("GameMode constructed"));
}

void ADBGameMode::BeginPlay()
{
    Super::BeginPlay();

    LastRateLimitReset = GetWorld()->GetTimeSeconds();

    UE_LOG(LogDBServer, Log, TEXT("GameMode BeginPlay - Server starting"));
    UE_LOG(LogDBServer, Log, TEXT("Backend URL: %s"), *BackendBaseURL);
    UE_LOG(LogDBServer, Log, TEXT("Max actions/sec: %d | Validation timeout: %.1fs"),
           MaxActionsPerSecond, ValidationTimeout);
}

void ADBGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Process pending validations (timeouts, retries)
    ProcessPendingValidations(DeltaTime);

    // Reset rate limiters every second
    const double CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastRateLimitReset >= 1.0)
    {
        ResetRateLimiters();
        LastRateLimitReset = CurrentTime;
    }

    // Process validation queue if we have capacity
    ProcessValidationQueue();
}

// ========================================
// PLAYER CONNECTION
// ========================================

void ADBGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    // Check server capacity before allowing connection
    const int32 CurrentPlayers = PlayerSessions.Num() + PendingValidations.Num();
    const int32 MaxPlayers = 100; // Configure this

    if (CurrentPlayers >= MaxPlayers)
    {
        ErrorMessage = TEXT("Server is full");
        UE_LOG(LogDBServer, Warning, TEXT("Rejected connection from %s - server full (%d/%d)"),
               *Address, CurrentPlayers, MaxPlayers);
        return;
    }

    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void ADBGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // Create empty session - will be populated on validation
    FDBPlayerSession NewSession;
    NewSession.bIsValidated = false;
    PlayerSessions.Add(NewPlayer, NewSession);

    UE_LOG(LogDBServer, Log, TEXT("Player connected: %s | Total: %d | Pending: %d"),
           *NewPlayer->GetName(),
           PlayerSessions.Num(),
           PendingValidations.Num());

    // Player needs to call Server_SendAuthToken to validate
    // Until then, they have limited permissions
}

void ADBGameMode::Logout(AController* Exiting)
{
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
    {
        CleanupPlayerSession(PC);

        UE_LOG(LogDBServer, Log, TEXT("Player disconnected | Remaining: %d"),
               PlayerSessions.Num());
    }

    Super::Logout(Exiting);
}

AActor* ADBGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // For MMO: Could load last logout position from backend
    // For now, use default behavior

    // Optionally spread players across multiple spawn points to avoid congestion
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    if (PlayerStarts.Num() > 0)
    {
        // Simple round-robin to spread players
        const int32 Index = PlayerSessions.Num() % PlayerStarts.Num();
        return PlayerStarts[Index];
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}

void ADBGameMode::CleanupPlayerSession(APlayerController* Player)
{
    // Save any persistent data to backend here before removing
    if (const FDBPlayerSession* Session = PlayerSessions.Find(Player))
    {
        if (Session->bIsValidated)
        {
            // TODO: Save player state to backend
            // SavePlayerStateToBackend(Player, Session->CharacterId);
        }
    }

    PlayerSessions.Remove(Player);
    PendingValidations.Remove(Player);

    // Remove from queue if present
    ValidationQueue.RemoveAll([Player](const TPair<TWeakObjectPtr<APlayerController>, FDBPendingValidation>& Pair)
    {
        return Pair.Key.Get() == Player;
    });
}

// ========================================
// TOKEN VALIDATION
// ========================================

void ADBGameMode::RequestJoinWithToken(APlayerController* Player, const FString& AuthToken, const FString& CharacterId)
{
    if (!Player)
    {
        UE_LOG(LogDBServer, Error, TEXT("RequestJoinWithToken - No player!"));
        return;
    }

    // Check if already validated
    if (const FDBPlayerSession* Existing = PlayerSessions.Find(Player))
    {
        if (Existing->bIsValidated)
        {
            UE_LOG(LogDBServer, Warning, TEXT("Player %s already validated, ignoring duplicate request"),
                   *Player->GetName());
            return;
        }
    }

    // Check if already pending
    if (PendingValidations.Contains(Player))
    {
        UE_LOG(LogDBServer, Warning, TEXT("Player %s already has pending validation"),
               *Player->GetName());
        return;
    }

    UE_LOG(LogDBServer, Log, TEXT("Player %s requesting join with character %s"),
           *Player->GetName(), *CharacterId);

    // Create pending validation
    FDBPendingValidation Pending;
    Pending.AuthToken = AuthToken;
    Pending.CharacterId = CharacterId;
    Pending.RequestTime = GetWorld()->GetTimeSeconds();
    Pending.RetryCount = 0;

    // Check if we have capacity for immediate processing
    if (ActiveBackendRequests < MaxConcurrentBackendRequests)
    {
        PendingValidations.Add(Player, Pending);
        ValidateTokenWithBackend(Player, Pending);
    }
    else
    {
        // Queue for later processing
        ValidationQueue.Add(TPair<TWeakObjectPtr<APlayerController>, FDBPendingValidation>(Player, Pending));
        UE_LOG(LogDBServer, Log, TEXT("Validation queued - backend at capacity (%d/%d)"),
               ActiveBackendRequests, MaxConcurrentBackendRequests);
    }
}

void ADBGameMode::ProcessValidationQueue()
{
    while (ValidationQueue.Num() > 0 && ActiveBackendRequests < MaxConcurrentBackendRequests)
    {
        auto& QueueEntry = ValidationQueue[0];
        APlayerController* Player = QueueEntry.Key.Get();

        if (Player && PlayerSessions.Contains(Player))
        {
            PendingValidations.Add(Player, QueueEntry.Value);
            ValidateTokenWithBackend(Player, QueueEntry.Value);
        }

        ValidationQueue.RemoveAt(0);
    }
}

void ADBGameMode::ValidateTokenWithBackend(APlayerController* Player, const FDBPendingValidation& Validation)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    FString URL = FString::Printf(TEXT("%s/api/auth/validate"), *BackendBaseURL);

    Request->SetURL(URL);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Validation.AuthToken));

    // Include character ID in body
    TSharedPtr<FJsonObject> JsonBody = MakeShared<FJsonObject>();
    JsonBody->SetStringField(TEXT("character_id"), Validation.CharacterId);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer);

    Request->SetContentAsString(JsonString);

    // Track active requests
    ActiveBackendRequests++;

    // Bind response handler
    Request->OnProcessRequestComplete().BindUObject(
        this,
        &ADBGameMode::OnTokenValidationResponse,
        Player,
        Validation.CharacterId
    );

    UE_LOG(LogDBServer, Log, TEXT("Validating token (active requests: %d)"), ActiveBackendRequests);
    Request->ProcessRequest();
}

void ADBGameMode::OnTokenValidationResponse(
    FHttpRequestPtr Request,
    FHttpResponsePtr Response,
    bool bWasSuccessful,
    APlayerController* Player,
    FString CharacterId)
{
    // Always decrement active requests
    ActiveBackendRequests = FMath::Max(0, ActiveBackendRequests - 1);

    // Check if player is still connected
    if (!Player || !PlayerSessions.Contains(Player))
    {
        UE_LOG(LogDBServer, Warning, TEXT("Player disconnected during validation"));
        PendingValidations.Remove(Player);
        return;
    }

    FDBPendingValidation* Pending = PendingValidations.Find(Player);

    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogDBServer, Error, TEXT("Token validation request failed"));

        // Retry logic
        if (Pending && Pending->RetryCount < MaxValidationRetries)
        {
            Pending->RetryCount++;
            UE_LOG(LogDBServer, Log, TEXT("Retrying validation (attempt %d/%d)"),
                   Pending->RetryCount, MaxValidationRetries);

            // Re-queue for retry
            ValidationQueue.Add(TPair<TWeakObjectPtr<APlayerController>, FDBPendingValidation>(Player, *Pending));
            PendingValidations.Remove(Player);
        }
        else
        {
            KickPlayer(Player, TEXT("Authentication failed - server error"));
        }
        return;
    }

    const int32 ResponseCode = Response->GetResponseCode();
    const FString ResponseContent = Response->GetContentAsString();

    UE_LOG(LogDBServer, Log, TEXT("Validation response: %d"), ResponseCode);

    if (ResponseCode == 200)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const FString PlayerId = JsonObject->GetStringField(TEXT("player_id"));

            // Update session
            if (FDBPlayerSession* Session = PlayerSessions.Find(Player))
            {
                Session->PlayerId = PlayerId;
                Session->CharacterId = CharacterId;
                Session->ValidationTime = GetWorld()->GetTimeSeconds();
                Session->bIsValidated = true;
                Session->LastActionTime = 0.0;
                Session->ActionsThisSecond = 0;
            }

            // Remove from pending
            PendingValidations.Remove(Player);

            UE_LOG(LogDBServer, Log, TEXT("Player validated! ID: %s, Character: %s"),
                   *PlayerId, *CharacterId);

            // Spawn their character
            SpawnPlayerCharacter(Player, CharacterId);
        }
        else
        {
            KickPlayer(Player, TEXT("Invalid server response"));
        }
    }
    else if (ResponseCode == 401 || ResponseCode == 403)
    {
        KickPlayer(Player, TEXT("Invalid or expired token"));
    }
    else
    {
        KickPlayer(Player, FString::Printf(TEXT("Authentication failed: %d"), ResponseCode));
    }
}

void ADBGameMode::ProcessPendingValidations(float DeltaTime)
{
    const double CurrentTime = GetWorld()->GetTimeSeconds();
    TArray<APlayerController*> TimedOut;

    for (auto& Pair : PendingValidations)
    {
        const double ElapsedTime = CurrentTime - Pair.Value.RequestTime;
        if (ElapsedTime > ValidationTimeout)
        {
            TimedOut.Add(Pair.Key);
        }
    }

    for (APlayerController* PC : TimedOut)
    {
        UE_LOG(LogDBServer, Warning, TEXT("Validation timed out for %s"), *PC->GetName());
        KickPlayer(PC, TEXT("Authentication timeout"));
    }
}

void ADBGameMode::SpawnPlayerCharacter(APlayerController* Player, const FString& CharacterId)
{
    UE_LOG(LogDBServer, Log, TEXT("Spawning character %s for player %s"),
           *CharacterId, *Player->GetName());

    // Character should already be spawned by default pawn system
    // Here we load their saved data

    if (ADBPlayerCharacter* Character = Cast<ADBPlayerCharacter>(Player->GetPawn()))
    {
        // TODO: Load from backend:
        // - Equipment/Inventory
        // - Current Job/Level
        // - Last position (or use spawn point)
        // - Abilities learned

        UE_LOG(LogDBServer, Log, TEXT("Character spawned and ready!"));
    }
    else
    {
        UE_LOG(LogDBServer, Warning, TEXT("No pawn for player - may need manual spawn"));
    }
}

void ADBGameMode::KickPlayer(APlayerController* Player, const FString& Reason)
{
    if (!Player) return;

    UE_LOG(LogDBServer, Warning, TEXT("Kicking player: %s - Reason: %s"),
           *Player->GetName(), *Reason);

    // Notify client before kicking
    if (ADBPlayerController* DBPC = Cast<ADBPlayerController>(Player))
    {
        DBPC->Client_ShowSystemMessage(FString::Printf(TEXT("Disconnected: %s"), *Reason));
    }

    // Cleanup session data
    CleanupPlayerSession(Player);

    // Use GameModeBase's built-in kick functionality
    // This properly handles the network disconnect
    if (UWorld* World = GetWorld())
    {
        if (AGameModeBase* GM = World->GetAuthGameMode())
        {
            GM->GameSession->KickPlayer(Player, FText::FromString(Reason));
        }
    }
}

// ========================================
// RATE LIMITING
// ========================================

bool ADBGameMode::CanPlayerPerformAction(APlayerController* Player, int32 ActionCost)
{
    if (!Player) return false;

    FDBPlayerSession* Session = PlayerSessions.Find(Player);
    if (!Session || !Session->bIsValidated)
    {
        UE_LOG(LogDBServer, Warning, TEXT("Unvalidated player %s attempted action"), *Player->GetName());
        return false;
    }

    // Check rate limit
    if (Session->ActionsThisSecond + ActionCost > MaxActionsPerSecond)
    {
        UE_LOG(LogDBServer, Warning, TEXT("Player %s rate limited (actions: %d/%d)"),
               *Player->GetName(), Session->ActionsThisSecond, MaxActionsPerSecond);
        return false;
    }

    Session->ActionsThisSecond += ActionCost;
    Session->LastActionTime = GetWorld()->GetTimeSeconds();
    return true;
}

void ADBGameMode::ResetRateLimiters()
{
    for (auto& Pair : PlayerSessions)
    {
        Pair.Value.ActionsThisSecond = 0;
    }
}

// ========================================
// SESSION ACCESS
// ========================================

bool ADBGameMode::IsPlayerValidated(APlayerController* Player) const
{
    if (const FDBPlayerSession* Session = PlayerSessions.Find(Player))
    {
        return Session->bIsValidated;
    }
    return false;
}

const FDBPlayerSession* ADBGameMode::GetPlayerSession(APlayerController* Player) const
{
    return PlayerSessions.Find(Player);
}

// ========================================
// COMBAT
// ========================================

void ADBGameMode::ApplyDamage(AActor* Target, float Damage, AActor* Initiator, AController* InstigatorController)
{
    if (!Target)
    {
        UE_LOG(LogDBServer, Warning, TEXT("ApplyDamage - No target!"));
        return;
    }

    // Rate limit check
    if (APlayerController* PC = Cast<APlayerController>(InstigatorController))
    {
        if (!CanPlayerPerformAction(PC, 1))
        {
            // Silently drop - could be speed hack attempt
            return;
        }
    }

    UE_LOG(LogDBServer, Log, TEXT("Applying %.1f damage to %s from %s"),
           Damage,
           *Target->GetName(),
           Initiator ? *Initiator->GetName() : TEXT("Unknown"));

    // TODO: Route through GAS for proper damage application
    // For now, notify victim

    if (ADBPlayerCharacter* TargetChar = Cast<ADBPlayerCharacter>(Target))
    {
        if (ADBPlayerController* TargetPC = Cast<ADBPlayerController>(TargetChar->GetController()))
        {
            TargetPC->Client_ShowDamageReceived(Damage);
        }
    }
}

// ========================================
// LOGGING HELPERS
// ========================================

void ADBGameMode::LogServer(const FString& Message) const
{
    UE_LOG(LogDBServer, Log, TEXT("[SERVER] %s"), *Message);
}

void ADBGameMode::LogServerWarning(const FString& Message) const
{
    UE_LOG(LogDBServer, Warning, TEXT("[SERVER] %s"), *Message);
}

void ADBGameMode::LogServerError(const FString& Message) const
{
    UE_LOG(LogDBServer, Error, TEXT("[SERVER] %s"), *Message);
}