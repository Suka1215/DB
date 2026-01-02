#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DBMMC_MagicDamage.generated.h"

UCLASS()
class DAWNBLADE_API UDBMMC_MagicDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
    
public:
	UDBMMC_MagicDamage();
    
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
    
protected:
	// Combat attributes
	FGameplayEffectAttributeCaptureDefinition MagicAccuracyDef;
	FGameplayEffectAttributeCaptureDefinition MagicEvasionDef;
    
	// Primary stats (for dINT/dMND calculations)
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	FGameplayEffectAttributeCaptureDefinition TargetIntelligenceDef;
	FGameplayEffectAttributeCaptureDefinition MindDef;
	FGameplayEffectAttributeCaptureDefinition TargetMindDef;
    
	// Magic skill levels
	FGameplayEffectAttributeCaptureDefinition ElementalMagicSkillDef;
	FGameplayEffectAttributeCaptureDefinition EnfeeblingMagicSkillDef;
	FGameplayEffectAttributeCaptureDefinition HealingMagicSkillDef;
	// Add more as needed: DarkMagicSkill, DivineMagicSkill, etc.
};