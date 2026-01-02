// DBWeaponAnimationSet.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/EDBJob.h"
#include "Abilities/GameplayAbility.h"
#include "DBWeaponAnimationSet.generated.h"

/**
 * Complete weapon animation and combat data
 * Data-driven weapon system - all weapons configured through this
 */
UCLASS(BlueprintType)
class DAWNBLADE_API UDBWeaponAnimationSet : public UDataAsset
{
    GENERATED_BODY()

public:
	
	// ========================================
	// JOB & LEVEL REQUIREMENTS
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	TArray<EDBJob> AllowedJobs; // Jobs that can use this weapon

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	int32 RequiredLevel; // Minimum level to equip

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	bool bAllJobsCanUse; // Override - any job can use (for starter weapons)
	
    // ========================================
    // WEAPON IDENTITY
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FName WeaponTypeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FGameplayTag WeaponCategoryTag; // Weapon.Melee.SwordAndShield, Weapon.Magic.Staff, etc.

    // ========================================
    // ANIMATION BLUEPRINT (Child of ABP_Unarmed)
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    TSubclassOf<UAnimInstance> AnimationBlueprint;

	// ========================================
	// ANIMATION TIMING (Child of ABP_Unarmed)
	// ========================================
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Timing", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float DrawMontagePlayRate = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Timing", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float SheathMontagePlayRate = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Timing", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float AttackMontagePlayRate = 1.0f;

	// Blend settings for smooth transitions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Blending", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MontageBlendInTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Blending", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MontageBlendOutTime = 0.25f;

	// ABP switch blend time (critical for smooth transitions)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Blending", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ABPSwitchBlendTime = 0.3f;

    // ========================================
    // LOCOMOTION - WEAPONS DRAWN
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion|Combat")
    UBlendSpace* CombatLocomotionBlendspace;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion|Combat")
    UAnimSequence* CombatIdle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion|Combat")
    bool bUsesStrafing;

    // ========================================
    // WEAPON DRAW/SHEATH MONTAGES
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
    UAnimMontage* DrawWeaponMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
    UAnimMontage* SheathWeaponMontage;

    // ========================================
    // COMBAT - AUTO ATTACKS
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Auto Attack")
    TArray<UAnimMontage*> BasicAttackMontages;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Auto Attack")
    float AutoAttackRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Auto Attack")
    float AutoAttackSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Auto Attack")
    float AutoAttackDamage;

    // ========================================
    // COMBAT - WEAPON SKILLS
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Skills")
    TArray<TSubclassOf<UGameplayAbility>> WeaponSkills;

    // ========================================
    // SOCKETS
    // ========================================
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets|MainHand")
	FName MainHandDrawnSocket; // sword_equipped_socket, staff_equipped_socket

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets|MainHand")
	FName MainHandSheathedSocket; // swordHolder, staffHolder

	// ========================================
	// SOCKETS - Off Hand (Shield, Secondary Weapon)
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets|OffHand")
	FName OffHandDrawnSocket; // shield_equipped_socket, dagger_equipped_socket

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets|OffHand")
	FName OffHandSheathedSocket; // shieldHolder, daggerHolder

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets|OffHand")
	bool bUsesOffHand; // Does this weapon style use off-hand? (sword+shield = true, staff = false)

    // ========================================
    // COMBAT BEHAVIOR
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    bool bRequiresTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    bool bCanAutoAttack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float EngageRange;

	// ========================================
	// VISUAL EFFECTS
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* TrailEffect; // Sword trail particle

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	class UNiagaraSystem* HitEffect; // Impact effect when hitting enemy

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FName TrailSocketName; // Where trail attaches on weapon

	// ========================================
	// SOUNDS
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* SwingSound; // Whoosh sound when attacking

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* HitSound; // Impact sound

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* DrawSound; // Sound when drawing weapon

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* SheathSound; // Sound when sheathing
};