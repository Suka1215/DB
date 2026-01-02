// DBAnimInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "DBAnimInstance.generated.h"

/**
 * Animation Instance for DawnBlade Characters
 * Reads Gameplay Tags from GAS to determine:
 * - Combat state (engaged, auto-attacking, casting)
 * - Weapon/Job type (staff, greatsword, sword+shield)
 * - Movement values (speed, direction)
 * 
 * Use this in your Animation Blueprint to drive:
 * - Base locomotion state machine (shared across all jobs)
 * - Layered Blend Per Bone for upper body combat stances (job-specific)
 */
UCLASS()
class DAWNBLADE_API UDBAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // ===== Combat State =====
    
    /** Is the character currently in combat? */
    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsInCombat = false;

    /** Is the character currently auto-attacking? */
    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsAutoAttacking = false;

    /** Is the character currently casting a spell? */
    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsCasting = false;

    /** Is the character dead? */
    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsDead = false;

    // ===== Weapon/Job State =====
    
    /** Has staff equipped (Light Mage) */
    UPROPERTY(BlueprintReadOnly, Category = "Weapon State")
    bool bHasStaffEquipped = false;

    /** Has great sword equipped (Warrior) */
    UPROPERTY(BlueprintReadOnly, Category = "Weapon State")
    bool bHasGreatSwordEquipped = false;

    /** Has sword and shield equipped (Paladin) */
    UPROPERTY(BlueprintReadOnly, Category = "Weapon State")
    bool bHasSwordShieldEquipped = false;

    // ===== Movement State =====
    
    /** Character's current movement speed */
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed = 0.f;

    /** Character's movement direction relative to rotation (-180 to 180) */
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Direction = 0.f;

    /** Is the character in the air? */
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInAir = false;

    /** Is the character moving? */
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsMoving = false;

    // ===== References =====
    
    /** Reference to owning character */
    UPROPERTY()
    ACharacter* OwningCharacter;

    /** Reference to Ability System Component */
    UPROPERTY()
    class UAbilitySystemComponent* AbilitySystemComponent;

    // ===== Update Functions =====
    
    /** Update combat state from gameplay tags */
    void UpdateCombatState();

    /** Update weapon/job state from gameplay tags */
    void UpdateWeaponState();

    /** Update movement values from character movement component */
    void UpdateMovementValues();
};