// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DBMMC_MagicDamage.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBMMC_MagicDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UDBMMC_MagicDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	FGameplayEffectAttributeCaptureDefinition MAtkDef;
	FGameplayEffectAttributeCaptureDefinition MDefDef;
	
};
