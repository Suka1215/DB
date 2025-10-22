#include "Systems/DBMMC_MagicDamage.h"
#include "Attributes/DBAttr_Combat.h"
#include "GameplayEffectExtension.h"

UDBMMC_MagicDamage::UDBMMC_MagicDamage()
{
	MAtkDef = FGameplayEffectAttributeCaptureDefinition(
		UDBAttr_Combat::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UDBAttr_Combat, MagicAttack)),
		EGameplayEffectAttributeCaptureSource::Source, true);
	MDefDef = FGameplayEffectAttributeCaptureDefinition(
		UDBAttr_Combat::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UDBAttr_Combat, MagicDefense)),
		EGameplayEffectAttributeCaptureSource::Target, true);
	RelevantAttributesToCapture.Add(MAtkDef);
	RelevantAttributesToCapture.Add(MDefDef);
}

float UDBMMC_MagicDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float A = 0.f, D = 0.f;
	GetCapturedAttributeMagnitude(MAtkDef, Spec, EvalParams, A);
	GetCapturedAttributeMagnitude(MDefDef, Spec, EvalParams, D);

	const float Power = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), false, 1.f);
	const float Ratio = FMath::Clamp((A + 1.f) / FMath::Max(1.f, D + 1.f), 0.5f, 1.6f);
	return Power * Ratio * 100.f;
}
