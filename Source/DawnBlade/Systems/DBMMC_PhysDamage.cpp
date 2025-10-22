#include "Systems/DBMMC_PhysDamage.h"
#include "Attributes/DBAttr_Combat.h"
#include "GameplayEffectExtension.h"

UDBMMC_PhysDamage::UDBMMC_PhysDamage()
{
	AttackDef = FGameplayEffectAttributeCaptureDefinition(
		UDBAttr_Combat::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UDBAttr_Combat, Attack)),
		EGameplayEffectAttributeCaptureSource::Source, true);
	DefenseDef = FGameplayEffectAttributeCaptureDefinition(
		UDBAttr_Combat::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UDBAttr_Combat, Defense)),
		EGameplayEffectAttributeCaptureSource::Target, true);
	RelevantAttributesToCapture.Add(AttackDef);
	RelevantAttributesToCapture.Add(DefenseDef);
}

float UDBMMC_PhysDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float A = 0.f, D = 0.f;
	GetCapturedAttributeMagnitude(AttackDef, Spec, EvalParams, A);
	GetCapturedAttributeMagnitude(DefenseDef, Spec, EvalParams, D);

	const float Power = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), /*WarnIfNotFound=*/false, /*Default*/1.f);
	const float Ratio = FMath::Clamp((A + 1.f) / FMath::Max(1.f, D + 1.f), 0.5f, 1.5f);
	return Power * Ratio * 100.f;
}
