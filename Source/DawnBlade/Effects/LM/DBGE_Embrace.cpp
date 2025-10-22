// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_Embrace.h"

#include "Attributes/DBAttr_Stats.h"

UDBGE_Embrace::UDBGE_Embrace()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Stats::GetHealthAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
