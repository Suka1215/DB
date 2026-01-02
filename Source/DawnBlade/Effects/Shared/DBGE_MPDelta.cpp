// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Shared/DBGE_MPDelta.h"

#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectExtension.h"
#include "Attributes/DBAttr_Primary.h"

UDBGE_MPDelta::UDBGE_MPDelta()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo MpMod;
	MpMod.Attribute = UDBAttr_Primary::GetManaAttribute();
	MpMod.ModifierOp = EGameplayModOp::Additive;
	MpMod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(MpMod);
}
