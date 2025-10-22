// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/Shared/DBGE_HPDelta.h"

#include "Attributes/DBAttr_Stats.h"

UDBGE_HPDelta::UDBGE_HPDelta()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Stats::GetHealthAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.HP"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
