// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Bolster.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Bolster : public UDBGA_Base
{
	GENERATED_BODY()
public:
	UDBGA_Bolster();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly) float Radius = 1200.f;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_Bolster; // Add Attack+Defense, duration via SBC
};
