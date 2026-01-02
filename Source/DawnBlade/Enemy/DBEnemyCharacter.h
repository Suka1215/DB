// DBEnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "Characters/DBCharacterBase.h"
#include "DBEnemyCharacter.generated.h"

class UDEnemyData;
class UDBAIStateComponent;
class UWidgetComponent;
struct FEnemyAttackPattern;

/** Delegate for enemy death - includes enemy reference */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, ADBEnemyCharacter*, Enemy);

/** Delegate for attack completed - for AI StateTree */
DECLARE_DELEGATE(FOnEnemyAttackCompleted);

/**
 * Enemy character class - inherits shared functionality from DBCharacterBase.
 * 
 * USAGE: Create Blueprint child classes and set mesh/anim/capsule in the Blueprint.
 * Stats come from the EnemyData asset assigned in Blueprint defaults.
 * 
 * Example hierarchy:
 *   ADBCharacterBase (C++)
 *     └── ADBEnemyCharacter (this class)
 *           ├── BP_Enemy_Goblin (mesh, anim, EnemyData = DA_Goblin)
 *           ├── BP_Enemy_Wolf (mesh, anim, EnemyData = DA_Wolf)
 *           └── BP_Enemy_Dragon (mesh, anim, EnemyData = DA_Dragon)
 */
UCLASS()
class DAWNBLADE_API ADBEnemyCharacter : public ADBCharacterBase
{
    GENERATED_BODY()

public:
    ADBEnemyCharacter();

    virtual void BeginPlay() override;
    
    // ========================================
    // REPLICATION
    // ========================================

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayMontage(UAnimMontage* Montage, float PlayRate = 1.0f);
    
    // ========================================
    // DELEGATES
    // ========================================

    /** Broadcast when this enemy dies - for spawner tracking */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeath OnEnemyDied;

    /** Called when attack montage finishes - for AI StateTree */
    FOnEnemyAttackCompleted OnAttackCompleted;

    // ========================================
    // COMPONENTS (Enemy-specific)
    // ========================================
    
    bool bIsAttacking = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UDBAIStateComponent> AIStateComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> HealthBarWidget;
    
    // ========================================
    // COMBAT STATE (for StateTree access)
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformAutoAttack();

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsArmed = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float NextAttackTime = 0.0f;

    // ========================================
    // DATA - Set in Blueprint defaults
    // ========================================

    /** Enemy stats and AI config - set this in your Blueprint child class */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
    TObjectPtr<UDEnemyData> EnemyData;

    // ========================================
    // PUBLIC GETTERS
    // ========================================

    UFUNCTION(BlueprintPure, Category = "Enemy")
    UDEnemyData* GetEnemyData() const { return EnemyData; }

    UFUNCTION(BlueprintPure, Category = "Enemy")
    bool IsAttacking() const { return bIsAttacking; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    // ========================================
    // COMBAT
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCurrentTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformAttack(const FEnemyAttackPattern& AttackPattern);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyDamageToEnemy(float Damage, AActor* DamageCauser);

protected:
    // ========================================
    // OVERRIDES FROM BASE
    // ========================================

    /** Apply stats from EnemyData - SERVER ONLY */
    virtual void InitClassDefaults() override;

    /** Handle death - SERVER ONLY */
    virtual void Death() override;

private:
    // ========================================
    // INTERNAL STATE
    // ========================================

    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY()
    TObjectPtr<AActor> CurrentTarget;

    FOnMontageEnded MontageEndedDelegate;
};