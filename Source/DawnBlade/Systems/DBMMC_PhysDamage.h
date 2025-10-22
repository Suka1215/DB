// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DBMMC_PhysDamage.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBMMC_PhysDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UDBMMC_PhysDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	FGameplayEffectAttributeCaptureDefinition AttackDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
};
