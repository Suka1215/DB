// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/DBPlayerController.h"
#include "Player/DBGameMode.h"
#include "Characters/DBPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogDBPlayerController, Log, All);

ADBPlayerController::ADBPlayerController()
{
    // Enable ticking for input batching on clients
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Slightly faster tick for responsive input
    PrimaryActorTick.TickInterval = 0.0f; // Full rate, we throttle ourselves
}

void ADBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Add any replicated properties here
    // DOREPLIFETIME(ADBPlayerController, SomeProperty);
}

void ADBPlayerController::BeginPlay()
{
    Super::BeginPlay();

    PrintNetworkRole(TEXT("BeginPlay"));

    // Client: Send auth token once we're ready
    if (IsLocalController() && !HasAuthority())
    {
        // Auth token should be set before this via SetAuthCredentials
        // Usually from login flow
        if (!CachedAuthToken.IsEmpty() && !bHasSentAuth)
        {
            Server_SendAuthToken(CachedAuthToken, CachedCharacterId);
            bHasSentAuth = true;
        }
    }
}

void ADBPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Client-side input batching
    if (IsLocalController() && !HasAuthority())
    {
        InputBatchTimer += DeltaTime;

        // Send batched inputs at fixed intervals or when buffer is full
        if ((InputBatchTimer >= InputBatchInterval && BufferedActions.Num() > 0) ||
            BufferedActions.Num() >= MaxBufferedActions)
        {
            FlushBufferedActions();
        }
    }
}

void ADBPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PrintNetworkRole(TEXT("OnPossess"));

    if (ADBPlayerCharacter* DBChar = Cast<ADBPlayerCharacter>(InPawn))
    {
        InitWidgetsForPawn(DBChar);
    }
}

void ADBPlayerController::OnRep_Pawn()
{
    Super::OnRep_Pawn();

    // Client-side pawn replication
    if (ADBPlayerCharacter* DBChar = Cast<ADBPlayerCharacter>(GetPawn()))
    {
        InitWidgetsForPawn(DBChar);
    }
}

// ========================================
// NETWORK HELPERS
// ========================================

void ADBPlayerController::PrintNetworkRole(const FString& Context)
{
    FString RoleStr;

    if (HasAuthority())
    {
        RoleStr = GetLocalRole() == ROLE_Authority ? TEXT("SERVER") : TEXT("SERVER (Listening)");
    }
    else
    {
        RoleStr = IsLocalController() ? TEXT("OWNING CLIENT") : TEXT("SIMULATED PROXY");
    }

    UE_LOG(LogDBPlayerController, Log, TEXT("[%s] %s - %s"),
           *RoleStr, *Context, *GetName());
}

ADBGameMode* ADBPlayerController::GetDBGameMode() const
{
    if (CachedGameMode.IsValid())
    {
        return CachedGameMode.Get();
    }

    // Only valid on server
    if (HasAuthority())
    {
        if (UWorld* World = GetWorld())
        {
            ADBPlayerController* MutableThis = const_cast<ADBPlayerController*>(this);
            MutableThis->CachedGameMode = Cast<ADBGameMode>(World->GetAuthGameMode());
            return MutableThis->CachedGameMode.Get();
        }
    }

    return nullptr;
}

// ========================================
// AUTHENTICATION
// ========================================

void ADBPlayerController::SetAuthCredentials(const FString& AuthToken, const FString& CharacterId)
{
    CachedAuthToken = AuthToken;
    CachedCharacterId = CharacterId;
}

bool ADBPlayerController::Server_SendAuthToken_Validate(const FString& AuthToken, const FString& CharacterId)
{
    // Basic validation - don't allow empty tokens
    return !AuthToken.IsEmpty() && !CharacterId.IsEmpty();
}

void ADBPlayerController::Server_SendAuthToken_Implementation(const FString& AuthToken, const FString& CharacterId)
{
    UE_LOG(LogDBPlayerController, Log, TEXT("[SERVER] Received auth token from %s for character %s"),
           *GetName(), *CharacterId);

    if (ADBGameMode* GM = GetDBGameMode())
    {
        GM->RequestJoinWithToken(this, AuthToken, CharacterId);
    }
    else
    {
        UE_LOG(LogDBPlayerController, Error, TEXT("[SERVER] No GameMode found!"));
    }
}

// ========================================
// INPUT BUFFERING
// ========================================

void ADBPlayerController::BufferAction(const FDBBufferedAction& Action)
{
    BufferedActions.Add(Action);

    // Force flush if buffer is full
    if (BufferedActions.Num() >= MaxBufferedActions)
    {
        FlushBufferedActions();
    }
}

void ADBPlayerController::FlushBufferedActions()
{
    if (BufferedActions.Num() == 0) return;

    // Send to server
    Server_BatchedInput(BufferedActions);

    // Clear buffer and reset timer
    BufferedActions.Reset();
    InputBatchTimer = 0.0f;
}

// ========================================
// SERVER RPC VALIDATION
// ========================================

bool ADBPlayerController::ValidateServerRPC(const FString& RPCName) const
{
    // Must have authority (running on server)
    if (!HasAuthority())
    {
        UE_LOG(LogDBPlayerController, Error, TEXT("RPC %s called but we don't have authority!"), *RPCName);
        return false;
    }

    // Check player is validated
    if (const ADBGameMode* GM = const_cast<ADBPlayerController*>(this)->GetDBGameMode())
    {
        if (!GM->IsPlayerValidated(const_cast<ADBPlayerController*>(this)))
        {
            UE_LOG(LogDBPlayerController, Warning, TEXT("Unvalidated player called %s"), *RPCName);
            return false;
        }
    }

    return true;
}

bool ADBPlayerController::ValidateTarget(AActor* Target, float MaxRange) const
{
    if (!Target) return false;

    // Check target is valid actor
    if (!IsValid(Target)) return false;

    // Range check if specified
    if (MaxRange > 0.0f && GetPawn())
    {
        const float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Target->GetActorLocation());
        if (Distance > MaxRange)
        {
            UE_LOG(LogDBPlayerController, Warning, TEXT("Target out of range: %.1f > %.1f"), Distance, MaxRange);
            return false;
        }
    }

    return true;
}

// ========================================
// SERVER RPCs - COMBAT
// ========================================

bool ADBPlayerController::Server_RequestAttack_Validate(AActor* Target)
{
    // Basic validation - target must exist
    return Target != nullptr;
}

void ADBPlayerController::Server_RequestAttack_Implementation(AActor* Target)
{
    if (!ValidateServerRPC(TEXT("Server_RequestAttack"))) return;

    // Validate target exists and is in range
    constexpr float AttackRange = 500.0f; // Configure this based on weapon
    if (!ValidateTarget(Target, AttackRange))
    {
        Client_ActionResult(0, false, TEXT("Invalid target"));
        return;
    }

    // Rate limiting
    if (ADBGameMode* GM = GetDBGameMode())
    {
        if (!GM->CanPlayerPerformAction(this, 1))
        {
            Client_ActionResult(0, false, TEXT("Too many actions"));
            return;
        }

        UE_LOG(LogDBPlayerController, Log, TEXT("[SERVER] %s attacking %s"),
               *GetName(), *Target->GetName());

        // Process attack through GAS or direct damage
        // GM->ApplyDamage(Target, BaseDamage, GetPawn(), this);

        // Visual feedback for all players
        if (GetPawn())
        {
            Multicast_PlayAttackEffect(GetPawn()->GetActorLocation(), GetPawn()->GetActorRotation());
        }
    }
}

bool ADBPlayerController::Server_RequestAbility_Validate(int32 AbilityIndex, AActor* Target)
{
    // Ability index must be reasonable
    return AbilityIndex >= 0 && AbilityIndex < 100;
}

void ADBPlayerController::Server_RequestAbility_Implementation(int32 AbilityIndex, AActor* Target)
{
    if (!ValidateServerRPC(TEXT("Server_RequestAbility"))) return;

    // Rate limiting (abilities cost more)
    if (ADBGameMode* GM = GetDBGameMode())
    {
        if (!GM->CanPlayerPerformAction(this, 2))
        {
            Client_ActionResult(0, false, TEXT("Too many actions"));
            return;
        }
    }

    UE_LOG(LogDBPlayerController, Log, TEXT("[SERVER] %s using ability %d on %s"),
           *GetName(), AbilityIndex, Target ? *Target->GetName() : TEXT("None"));

    // Route through GAS
    if (ADBPlayerCharacter* Char = Cast<ADBPlayerCharacter>(GetPawn()))
    {
        // Char->TryActivateBySlot(AbilityIndex);
        // Or use the target-based activation
    }
}

void ADBPlayerController::Server_RequestMoveToLocation_Implementation(FVector Destination)
{
    // Movement is handled by CharacterMovementComponent replication
    // This is for click-to-move style games

    UE_LOG(LogDBPlayerController, Verbose, TEXT("[SERVER] %s move request to %s"),
           *GetName(), *Destination.ToString());

    // Could implement pathfinding here
}

void ADBPlayerController::Server_BatchedInput_Implementation(const TArray<FDBBufferedAction>& Actions)
{
    if (!ValidateServerRPC(TEXT("Server_BatchedInput"))) return;

    // Rate limit check for batch
    ADBGameMode* GM = GetDBGameMode();
    if (!GM) return;

    for (const FDBBufferedAction& Action : Actions)
    {
        // Check rate limit for each action
        if (!GM->CanPlayerPerformAction(this, 1))
        {
            UE_LOG(LogDBPlayerController, Warning, TEXT("Batch action dropped - rate limited"));
            break;
        }

        switch (Action.ActionType)
        {
        case 0: // Attack
            if (Action.Target.IsValid())
            {
                Server_RequestAttack_Implementation(Action.Target.Get());
            }
            break;

        case 1: // Ability
            Server_RequestAbility_Implementation(Action.AbilityIndex, Action.Target.Get());
            break;

        case 2: // Move
            Server_RequestMoveToLocation_Implementation(Action.Location);
            break;

        default:
            UE_LOG(LogDBPlayerController, Warning, TEXT("Unknown action type: %d"), Action.ActionType);
            break;
        }
    }
}

// ========================================
// CLIENT RPCs
// ========================================

void ADBPlayerController::Client_ShowDamageDealt_Implementation(float Damage, FVector WorldLocation)
{
    UE_LOG(LogDBPlayerController, Log, TEXT("[CLIENT] Damage dealt: %.1f at %s"),
           Damage, *WorldLocation.ToString());

    // TODO: Show floating damage number widget
    // Create widget at WorldLocation projected to screen
}

void ADBPlayerController::Client_ShowDamageReceived_Implementation(float Damage)
{
    UE_LOG(LogDBPlayerController, Log, TEXT("[CLIENT] Damage received: %.1f"), Damage);

    // TODO: Flash screen red, show damage indicator
    // Update health bar
}

void ADBPlayerController::Client_ShowSystemMessage_Implementation(const FString& Message)
{
    UE_LOG(LogDBPlayerController, Log, TEXT("[CLIENT] System: %s"), *Message);

    // TODO: Show in chat/notification area
}

void ADBPlayerController::Client_ActionResult_Implementation(int32 SequenceNumber, bool bSuccess, const FString& FailReason)
{
    if (!bSuccess)
    {
        UE_LOG(LogDBPlayerController, Log, TEXT("[CLIENT] Action %d failed: %s"),
               SequenceNumber, *FailReason);

        // TODO: Could trigger client-side rollback for prediction
    }
}

// ========================================
// MULTICAST RPCs
// ========================================

void ADBPlayerController::Multicast_PlayAttackEffect_Implementation(FVector Location, FRotator Rotation)
{
    // Don't play on dedicated server
    if (GetNetMode() == NM_DedicatedServer) return;

    UE_LOG(LogDBPlayerController, Verbose, TEXT("Playing attack effect at %s"), *Location.ToString());

    // TODO: Spawn VFX at location
    // UGameplayStatics::SpawnEmitterAtLocation(...)
}

void ADBPlayerController::Multicast_PlayAbilitySound_Implementation(FVector Location, USoundBase* Sound)
{
    // Don't play on dedicated server
    if (GetNetMode() == NM_DedicatedServer) return;

    if (Sound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
    }
}

void ADBPlayerController::Multicast_ShowCombatText_Implementation(FVector Location, const FString& Text, FColor Color)
{
    // Don't process on dedicated server
    if (GetNetMode() == NM_DedicatedServer) return;

    UE_LOG(LogDBPlayerController, Verbose, TEXT("Combat text: %s at %s"), *Text, *Location.ToString());

    // TODO: Spawn floating combat text widget
}

// ========================================
// UI MANAGEMENT
// ========================================

void ADBPlayerController::CreateOrShowHUD()
{
    // Only create UI on local client
    if (!IsLocalController()) return;

    if (!GameHUDInstance && GameHUDClass)
    {
        GameHUDInstance = CreateWidget<UUserWidget>(this, GameHUDClass);
        if (GameHUDInstance)
        {
            GameHUDInstance->AddToViewport();
        }
    }

    if (!DebugPanelInstance && DebugPanelClass)
    {
        DebugPanelInstance = CreateWidget<UUserWidget>(this, DebugPanelClass);
        if (DebugPanelInstance)
        {
            DebugPanelInstance->AddToViewport();
            DebugPanelInstance->SetVisibility(ESlateVisibility::Hidden); // Toggle with key
        }
    }
}

void ADBPlayerController::InitWidgetsForPawn(ADBPlayerCharacter* PC)
{
    if (!IsLocalController()) return;

    CreateOrShowHUD();

    // Initialize widgets with character reference
    // if (UDBW_GameHUD* HUD = Cast<UDBW_GameHUD>(GameHUDInstance))
    // {
    //     HUD->InitForCharacter(PC);
    // }
}