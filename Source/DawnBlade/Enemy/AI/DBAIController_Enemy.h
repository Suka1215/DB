// DBAIController_Enemy.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "DBAIController_Enemy.generated.h"

/**
 * Enemy AI Controller using StateTree (UE5's modern AI system)
 * Pattern based on Epic's Lyra and community best practices
 */
UCLASS()
class DAWNBLADE_API ADBAIController_Enemy : public AAIController
{
    GENERATED_BODY()

public:
    ADBAIController_Enemy();

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // ========================================
    // COMPONENTS
    // ========================================

    /** StateTree Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UStateTreeComponent* StateTreeComponent;

    /** AI Perception */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerception;

    /** Sight sense config */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UAISenseConfig_Sight* SightConfig;

    /** Damage sense config */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UAISenseConfig_Damage* DamageConfig;

    // ========================================
    // STATE TREE
    // ========================================

    /** Default StateTree asset */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    class UStateTree* DefaultStateTree;

    // ========================================
    // TARGET MANAGEMENT
    // ========================================

    /** Set the current target actor */
    UFUNCTION(BlueprintCallable, Category = "AI|Target")
    void SetTargetActor(AActor* NewTarget);

    /** Get the current target actor */
    UFUNCTION(BlueprintPure, Category = "AI|Target")
    AActor* GetTargetActor() const { return CurrentTarget; }

    /** Get last known target location */
    UFUNCTION(BlueprintPure, Category = "AI|Target")
    FVector GetLastKnownTargetLocation() const { return LastKnownTargetLocation; }

    /** Update last known target location */
    void UpdateLastKnownTargetLocation();

    /** Clear target */
    UFUNCTION(BlueprintCallable, Category = "AI|Target")
    void ClearTarget();

    // ========================================
    // LOCATION MANAGEMENT
    // ========================================

    /** Get home location (spawn point) */
    UFUNCTION(BlueprintPure, Category = "AI|Location")
    FVector GetHomeLocation() const { return HomeLocation; }

    /** Set home location */
    UFUNCTION(BlueprintCallable, Category = "AI|Location")
    void SetHomeLocation(const FVector& Location) { HomeLocation = Location; }

    // ========================================
    // PERCEPTION
    // ========================================

    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    UFUNCTION()
    void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION(BlueprintCallable, Category = "AI")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UFUNCTION(BlueprintCallable, Category = "AI")
    UDBEnemyStateTreeContext* GetStateTreeContext() const { return StateTreeContext; }

    UFUNCTION(BlueprintPure, Category = "AI")
    class ADBEnemyCharacter* GetEnemyCharacter() const;

    UFUNCTION(BlueprintCallable, Category = "AI")
    UDEnemyData* GetCachedEnemyData() const { return CachedEnemyData; }

protected:
    void SetupPerception();
    void InitializeAI(ADBEnemyCharacter* EnemyChar);

    void CallNearbyAllies(float LinkRadius);
    void DrawDetectionDebug(ADBEnemyCharacter* EnemyChar, AActor* Target, float Distance);

    UPROPERTY()
    TObjectPtr<UDBEnemyStateTreeContext> StateTreeContext;

    /** Current target actor */
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    TObjectPtr<AActor> CurrentTarget;

    /** Last known target location */
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    FVector LastKnownTargetLocation;

    /** Home location (spawn point) */
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    FVector HomeLocation;

    UPROPERTY()
    TObjectPtr<UDEnemyData> CachedEnemyData;

    bool bShowDebugLines = false;
};