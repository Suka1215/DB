// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Charge.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Charge : public UDBGA_Base
{
	GENERATED_BODY()
public:
	UDBGA_Charge();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly) float DashStrength = 1200.f;
	UPROPERTY(EditDefaultsOnly) float HitRange = 180.f;
	UPROPERTY(EditDefaultsOnly) float StaggerSeconds = 0.6f;
	UPROPERTY(EditDefaultsOnly) float Power = 0.7f;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_PhysDamage;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_Stagger;	
};
