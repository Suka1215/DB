// ============================================================================
// DBGA_Engage.h - Add Helper Method
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DBGA_Engage.generated.h"

class UAnimMontage;
class ACharacter;

UCLASS()
class DAWNBLADE_API UDBGA_Engage : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDBGA_Engage();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	/** Montage to play when engaging */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Engage")
	TObjectPtr<UAnimMontage> EngageMontage;

	/** Helper to play montage after ABP switch */
	void PlayEngageMontage(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		ACharacter* Character);

	/** Called when montage completes successfully */
	UFUNCTION()
	void OnMontageCompleted();

	/** Called when montage is interrupted */
	UFUNCTION()
	void OnMontageInterrupted();

	/** Called when montage is cancelled */
	UFUNCTION()
	void OnMontageCancelled();
};