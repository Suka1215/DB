// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/IHttpRequest.h"
#include "DBGameMode.generated.h"

class ADBPlayerCharacter;

/**
 * Cached player session data - avoid repeated backend calls
 */
USTRUCT()
struct FDBPlayerSession
{
    GENERATED_BODY()

    UPROPERTY()
    FString PlayerId;

    UPROPERTY()
    FString CharacterId;

    UPROPERTY()
    double ValidationTime = 0.0;

    UPROPERTY()
    bool bIsValidated = false;

    // Rate limiting
    double LastActionTime = 0.0;
    int32 ActionsThisSecond = 0;
};

/**
 * Pending validation request
 */
USTRUCT()
struct FDBPendingValidation
{
    GENERATED_BODY()

    UPROPERTY()
    FString AuthToken;

    UPROPERTY()
    FString CharacterId;

    UPROPERTY()
    double RequestTime = 0.0;

    int32 RetryCount = 0;
};

UCLASS()
class DAWNBLADE_API ADBGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADBGameMode();

    // ========================================
    // LIFECYCLE
    // ========================================

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ========================================
    // PLAYER CONNECTION (Server Only)
    // ========================================

    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

    /**
     * Called by client when they want to join with their auth token
     * Server will validate this token against the web backend
     */
    void RequestJoinWithToken(APlayerController* Player, const FString& AuthToken, const FString& CharacterId);

    /**
     * Check if a player is fully validated and ready to play
     */
    UFUNCTION(BlueprintPure, Category = "Auth")
    bool IsPlayerValidated(APlayerController* Player) const;

    /**
     * Get player's session data (returns nullptr if not validated)
     */
    const FDBPlayerSession* GetPlayerSession(APlayerController* Player) const;

    // ========================================
    // RATE LIMITING (Anti-Cheat Foundation)
    // ========================================

    /**
     * Check if player can perform an action (rate limited)
     * @param Player - The player attempting the action
     * @param ActionCost - How many "action points" this costs (attack=1, ability=2, etc)
     * @return true if action is allowed
     */
    bool CanPlayerPerformAction(APlayerController* Player, int32 ActionCost = 1);

    // ========================================
    // COMBAT (Server Authoritative)
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyDamage(AActor* Target, float Damage, AActor* Initiator, AController* InstigatorController);

    // ========================================
    // CONFIGURATION
    // ========================================

    /** Your web backend URL */
    UPROPERTY(EditDefaultsOnly, Category = "Backend")
    FString BackendBaseURL = TEXT("http://localhost:3000");

    /** How long a validation is cached before requiring re-auth (seconds) */
    UPROPERTY(EditDefaultsOnly, Category = "Backend")
    float ValidationCacheDuration = 3600.0f; // 1 hour

    /** Max actions per second per player (rate limiting) */
    UPROPERTY(EditDefaultsOnly, Category = "Security")
    int32 MaxActionsPerSecond = 30;

    /** Timeout for pending validations (seconds) */
    UPROPERTY(EditDefaultsOnly, Category = "Backend")
    float ValidationTimeout = 30.0f;

    /** Max retry attempts for failed validations */
    UPROPERTY(EditDefaultsOnly, Category = "Backend")
    int32 MaxValidationRetries = 3;

    /** Max concurrent HTTP requests to backend */
    UPROPERTY(EditDefaultsOnly, Category = "Backend")
    int32 MaxConcurrentBackendRequests = 10;

    /** Server tick rate for player updates (Hz) - lower = less CPU, less responsive */
    UPROPERTY(EditDefaultsOnly, Category = "Performance")
    float PlayerUpdateRate = 20.0f; // 20 Hz = 50ms intervals

protected:
    // ========================================
    // PLAYER TRACKING
    // ========================================

    /** All connected players with their session data */
    UPROPERTY()
    TMap<APlayerController*, FDBPlayerSession> PlayerSessions;

    /** Players waiting for validation */
    UPROPERTY()
    TMap<APlayerController*, FDBPendingValidation> PendingValidations;

    /** Current number of active backend requests */
    int32 ActiveBackendRequests = 0;

    /** Queue of validation requests waiting for capacity */
    TArray<TPair<TWeakObjectPtr<APlayerController>, FDBPendingValidation>> ValidationQueue;

    // ========================================
    // TICK OPTIMIZATION
    // ========================================

    /** Time accumulator for batched operations */
    float TickAccumulator = 0.0f;

    /** Process pending validations and timeouts */
    void ProcessPendingValidations(float DeltaTime);

    /** Reset rate limiters each second */
    void ResetRateLimiters();
    double LastRateLimitReset = 0.0;

private:
    // ========================================
    // BACKEND COMMUNICATION
    // ========================================

    void ValidateTokenWithBackend(APlayerController* Player, const FDBPendingValidation& Validation);
    void OnTokenValidationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* Player, FString CharacterId);
    void ProcessValidationQueue();

    // ========================================
    // PLAYER MANAGEMENT
    // ========================================

    void SpawnPlayerCharacter(APlayerController* Player, const FString& CharacterId);
    void KickPlayer(APlayerController* Player, const FString& Reason);
    void CleanupPlayerSession(APlayerController* Player);

    // ========================================
    // LOGGING HELPERS
    // ========================================

    void LogServer(const FString& Message) const;
    void LogServerWarning(const FString& Message) const;
    void LogServerError(const FString& Message) const;
};