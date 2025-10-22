// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_Grace.h"

#include "Attributes/DBAttr_Stats.h"

UDBGE_Grace::UDBGE_Grace()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Period = 1.f; // tick interval (overridden in GA)

	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Stats::GetHealthAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
