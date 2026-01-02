// DBGA_AutoAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_AutoAttack.generated.h"

/**
 * FFXI-Style Auto-Attack Ability
 */
UCLASS()
class DAWNBLADE_API UDBGA_AutoAttack : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_AutoAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// ===== Attack Configuration =====
    
	UPROPERTY(EditDefaultsOnly, Category = "Auto Attack|Range")
	float MeleeRange = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Auto Attack|Animation")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Auto Attack|Timing")
	float BaseAttackInterval = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Auto Attack|Damage")
	TSubclassOf<class UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Auto Attack|Damage")
	float BaseDamage = 10.f;

	// ===== Runtime State =====
    
	FTimerHandle AutoAttackTimerHandle;
	int32 CurrentAttackIndex = 0;

	// ===== Core Functions =====
    
	UFUNCTION()
	void PerformAutoAttack();

	void ScheduleNextAttack();
	float GetAttackInterval() const;
	bool IsTargetInRange() const;
	AActor* GetCurrentTarget() const;
	void ApplyDamageToTarget(AActor* Target);

	// ===== Montage Callbacks (FIXED: No parameters) =====
    
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageBlendOut();

	UFUNCTION()
	void OnMontageCancelled();
};