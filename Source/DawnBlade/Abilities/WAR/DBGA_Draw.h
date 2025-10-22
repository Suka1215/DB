// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Draw.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Draw : public UDBGA_Base
{
	GENERATED_BODY()
public:
	UDBGA_Draw();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly) float ForcedSeconds = 6.f;  // force-target window
	UPROPERTY(EditDefaultsOnly) float ThreatBonus   = 500.f;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_Provoke; // grants State.ForcedTarget on target
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UGameplayEffect> GE_Enmity;  // instant threat add (Power SBC)
};
