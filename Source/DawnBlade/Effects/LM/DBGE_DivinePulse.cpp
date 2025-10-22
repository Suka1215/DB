// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_DivinePulse.h"

#include "Attributes/DBAttr_Stats.h"

UDBGE_DivinePulse::UDBGE_DivinePulse()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// HP sacrifice (negative)
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UDBAttr_Stats::GetHealthAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;

		FSetByCallerFloat SBC;
		SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.HP"));
		Mod.ModifierMagnitude = SBC;

		Modifiers.Add(Mod);
	}

	// MP restore
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UDBAttr_Stats::GetManaAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;

		FSetByCallerFloat SBC;
		SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Mana"));
		Mod.ModifierMagnitude = SBC;

		Modifiers.Add(Mod);
	}
}
