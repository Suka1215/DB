// DBCombatComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DBCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatStateChanged);

/**
 * Combat Component - Manages targeting, engagement, and auto-attack state
 * FFXI-style: Engage target -> auto-attack begins automatically
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDBCombatComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ===== Target Management =====
    
    /** Set the current target */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetTarget(AActor* NewTarget);

    /** Get current target */
    UFUNCTION(BlueprintPure, Category = "Combat")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    // ===== Combat Engagement =====
    
    /** Engage target and start auto-attack */
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Combat")
    void ServerEngageTarget(AActor* Target);

    /** Disengage from combat */
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Combat")
    void ServerDisengage();

    /** Check if currently in combat */
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsInCombat() const;

    /** Check if currently auto-attacking */
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsAutoAttacking() const;

    // ===== Events =====
    
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnTargetChanged OnTargetChanged;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCombatStateChanged OnCombatStateChanged;

    // ===== Configuration =====
    
    /** Auto-disengage if target is further than this distance */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Settings")
    float AutoDisengageDistance = 2000.f;

    /** How often to check target validity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Settings")
    float TargetCheckInterval = 0.5f;

    // ===== Target Indicator UI =====
    
    /** Widget class for target indicator (arrow/icon above enemy) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|UI")
    TSubclassOf<class UUserWidget> TargetIndicatorClass;

protected:
    virtual void BeginPlay() override;

    /** Current target actor */
    UPROPERTY(ReplicatedUsing=OnRep_CurrentTarget)
    TObjectPtr<AActor> CurrentTarget;

    UFUNCTION()
    void OnRep_CurrentTarget();

    /** Reference to owner's AbilitySystemComponent */
    UPROPERTY()
    class UAbilitySystemComponent* AbilitySystemComponent;

    /** Timer for target validation checks */
    float TimeSinceLastTargetCheck = 0.f;

    /** Validate target is still valid (alive, in range, etc.) */
    void CheckTargetValidity();

    /** Start auto-attack ability */
    void StartAutoAttack();

    /** Stop auto-attack ability */
    void StopAutoAttack();

    /** Create and manage target indicator widget */
    void UpdateTargetIndicator();

    /** Current target indicator widget component */
    UPROPERTY()
    class UWidgetComponent* TargetIndicatorWidget;
};