#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/FDBAbilityData.h"
#include "DBGA_Disengage.generated.h"

/**
 * Disengage - Sheathe weapons and return to idle stance
 * Reads montage and other data from FDBAbilityData
 */
UCLASS()
class DAWNBLADE_API UDBGA_Disengage : public UGameplayAbility
{
	GENERATED_BODY()
    
public:
	UDBGA_Disengage();
    
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
    
	// Ability data from data table - set when granting the ability
	UPROPERTY(BlueprintReadWrite, Category = "Ability Data")
	FDBAbilityData AbilityData;
    
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disengage")
	UAnimMontage* DisengageMontage;
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
};