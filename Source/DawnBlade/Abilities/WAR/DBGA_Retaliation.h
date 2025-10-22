// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Retaliation.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Retaliation : public UDBGA_Base
{
	GENERATED_BODY()
public:
	UDBGA_Retaliation();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_RetaliationBuff; // grants State.Counter100
};
