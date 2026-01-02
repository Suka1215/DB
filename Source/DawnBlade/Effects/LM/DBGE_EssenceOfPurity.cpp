// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/LM/DBGE_EssenceOfPurity.h"

#include "Attributes/DBAttr_Primary.h"

UDBGE_EssenceOfPurity::UDBGE_EssenceOfPurity()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Optional: small heal on cleanse
	FGameplayModifierInfo Mod;
	Mod.Attribute = UDBAttr_Primary::GetHealthAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing"));
	Mod.ModifierMagnitude = SBC;

	Modifiers.Add(Mod);
}
