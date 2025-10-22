// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Annihilation.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Annihilation : public UDBGA_Base
{
	GENERATED_BODY()
public:
	UDBGA_Annihilation();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly) float Radius = 40.f;
	UPROPERTY(EditDefaultsOnly) float BasePower = 1.2f; // multiplied by (1 + TP/TPScale)
	UPROPERTY(EditDefaultsOnly) float TPScale   = 1000.f; // 1000 TP = +100% base
	UPROPERTY(EditDefaultsOnly) float BonusEnmity = 300.f;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_PhysDamage;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_TPDelta; // Instant TP delta (negative to spend)
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_Enmity;  // Instant threat add (Power via SBC)
};
