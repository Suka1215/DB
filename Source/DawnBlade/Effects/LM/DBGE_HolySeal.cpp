// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_HolySeal.h"

#include "Attributes/DBAttr_Combat.h"

UDBGE_HolySeal::UDBGE_HolySeal()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Combat::GetMagicAttackAttribute(); // boost healing via magic power
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.HealingMultiplier"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
