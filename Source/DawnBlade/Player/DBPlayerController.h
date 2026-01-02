// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DBPlayerController.generated.h"

class UUserWidget;
class UDBW_GameHUD;
class UDBW_AbilityDebugPanel;
class ADBPlayerCharacter;
class ADBGameMode;

/**
 * Buffered input action for client-side prediction
 */
USTRUCT()
struct FDBBufferedAction
{
    GENERATED_BODY()

    UPROPERTY()
    int32 ActionType = 0; // 0=Attack, 1=Ability, 2=Move

    UPROPERTY()
    int32 AbilityIndex = -1;

    UPROPERTY()
    TWeakObjectPtr<AActor> Target;

    UPROPERTY()
    FVector Location = FVector::ZeroVector;

    UPROPERTY()
    double Timestamp = 0.0;

    UPROPERTY()
    int32 SequenceNumber = 0;
};

UCLASS()
class DAWNBLADE_API ADBPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ADBPlayerController();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;

    // ========================================
    // NETWORK ROLE HELPERS
    // ========================================

    /** Print where this code is running - helps debug networking */
    UFUNCTION(BlueprintCallable, Category = "Network|Debug")
    void PrintNetworkRole(const FString& Context);

    /** Get cached GameMode (server only) */
    ADBGameMode* GetDBGameMode() const;

    // ========================================
    // AUTHENTICATION
    // ========================================

    /**
     * Called by client after connecting to send auth token to server
     * This proves to the server that we're a legitimate logged-in player
     */
    UFUNCTION(Server, Reliable, WithValidation, Category = "Auth")
    void Server_SendAuthToken(const FString& AuthToken, const FString& CharacterId);

    /** Client-side: Store auth credentials for reconnection */
    void SetAuthCredentials(const FString& AuthToken, const FString& CharacterId);

    // ========================================
    // SERVER RPCs - Client calls, Server executes
    // ========================================

    /**
     * Request to attack a target
     * Client: "Server, I want to attack this enemy"
     * Server: Validates and processes the attack
     */
    UFUNCTION(Server, Reliable, WithValidation, Category = "Combat")
    void Server_RequestAttack(AActor* Target);

    /**
     * Request to use an ability
     * Client: "Server, I want to use ability X on target Y"
     */
    UFUNCTION(Server, Reliable, WithValidation, Category = "Combat")
    void Server_RequestAbility(int32 AbilityIndex, AActor* Target);

    /**
     * Request to move to location (for click-to-move if you add it)
     * Unreliable because movement updates frequently
     */
    UFUNCTION(Server, Unreliable, Category = "Movement")
    void Server_RequestMoveToLocation(FVector Destination);

    /**
     * Batched input update - more efficient than individual RPCs
     * Sends multiple inputs in one packet
     */
    UFUNCTION(Server, Unreliable, Category = "Input")
    void Server_BatchedInput(const TArray<FDBBufferedAction>& Actions);

    // ========================================
    // CLIENT RPCs - Server calls, Owning Client executes
    // ========================================

    /**
     * Server tells THIS client to show a damage number
     * Only the attacking player sees "You dealt 50 damage!"
     */
    UFUNCTION(Client, Reliable, Category = "UI")
    void Client_ShowDamageDealt(float Damage, FVector WorldLocation);

    /**
     * Server tells THIS client to show damage received
     * Only you see "You took 30 damage!"
     */
    UFUNCTION(Client, Reliable, Category = "UI")
    void Client_ShowDamageReceived(float Damage);

    /**
     * Server tells THIS client about a system message
     * "You don't have enough MP" / "Target is too far"
     */
    UFUNCTION(Client, Reliable, Category = "UI")
    void Client_ShowSystemMessage(const FString& Message);

    /**
     * Server confirms/rejects an action (for client prediction reconciliation)
     */
    UFUNCTION(Client, Reliable, Category = "Prediction")
    void Client_ActionResult(int32 SequenceNumber, bool bSuccess, const FString& FailReason);

    // ========================================
    // MULTICAST RPCs - Server calls, ALL Clients execute
    // ========================================

    /**
     * Everyone sees the attack visual effect
     * Unreliable because visual effects can be dropped without gameplay impact
     */
    UFUNCTION(NetMulticast, Unreliable, Category = "Effects")
    void Multicast_PlayAttackEffect(FVector Location, FRotator Rotation);

    /**
     * Everyone hears the ability sound
     */
    UFUNCTION(NetMulticast, Unreliable, Category = "Effects")
    void Multicast_PlayAbilitySound(FVector Location, USoundBase* Sound);

    /**
     * Combat text that everyone nearby can see (critical hits, etc)
     */
    UFUNCTION(NetMulticast, Unreliable, Category = "Effects")
    void Multicast_ShowCombatText(FVector Location, const FString& Text, FColor Color);

    // ========================================
    // CLIENT-SIDE PREDICTION
    // ========================================

    /** Buffer an action for sending to server */
    void BufferAction(const FDBBufferedAction& Action);

    /** Get current sequence number for action tracking */
    int32 GetNextSequenceNumber() { return ++ActionSequenceNumber; }

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnRep_Pawn() override;

    // ========================================
    // CONFIGURATION
    // ========================================

    /** How often to send batched inputs (seconds) */
    UPROPERTY(EditDefaultsOnly, Category = "Network")
    float InputBatchInterval = 0.05f; // 20 Hz

    /** Max actions to buffer before force-sending */
    UPROPERTY(EditDefaultsOnly, Category = "Network")
    int32 MaxBufferedActions = 10;

    // ========================================
    // UI
    // ========================================

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameHUDClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> DebugPanelClass;

private:
    // ========================================
    // INPUT BUFFERING (Client-side)
    // ========================================

    /** Actions waiting to be sent to server */
    TArray<FDBBufferedAction> BufferedActions;

    /** Time since last input batch was sent */
    float InputBatchTimer = 0.0f;

    /** Sequence number for action tracking */
    int32 ActionSequenceNumber = 0;

    /** Send buffered actions to server */
    void FlushBufferedActions();

    // ========================================
    // AUTH STATE (Client-side)
    // ========================================

    FString CachedAuthToken;
    FString CachedCharacterId;
    bool bHasSentAuth = false;

    // ========================================
    // CACHING
    // ========================================

    /** Cached GameMode reference (server only) */
    UPROPERTY()
    TWeakObjectPtr<ADBGameMode> CachedGameMode;

    // ========================================
    // UI MANAGEMENT
    // ========================================

    UPROPERTY()
    TObjectPtr<UUserWidget> GameHUDInstance = nullptr;

    UPROPERTY()
    TObjectPtr<UUserWidget> DebugPanelInstance = nullptr;

    void CreateOrShowHUD();
    void InitWidgetsForPawn(ADBPlayerCharacter* PC);

    // ========================================
    // VALIDATION HELPERS
    // ========================================

    /** Basic validation for server RPCs */
    bool ValidateServerRPC(const FString& RPCName) const;

    /** Validate target is legitimate */
    bool ValidateTarget(AActor* Target, float MaxRange = 0.0f) const;
};