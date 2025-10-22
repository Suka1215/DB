// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_MysticBoon.h"

#include "Attributes/DBAttr_Combat.h"

UDBGE_MysticBoon::UDBGE_MysticBoon()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Combat::GetMagicAttackAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.HealingMultiplier"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
