// DBGE_Cure.cpp

#include "Effects/Spells/DBGE_Cure.h"

#include "GameplayTagContainer.h"      // FGameplayTag
#include "GameplayEffectTypes.h"       // FSetByCallerFloat, FGameplayModifierInfo
#include "Attributes/DBAttr_Stats.h"   // UDBAttr_Stats::GetHealthAttribute()

UDBGE_Cure::UDBGE_Cure()
{
	// Instant (no duration, no periodic ticking)
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Single modifier: add to Health using a SetByCaller magnitude
	FGameplayModifierInfo HealMod;
	HealMod.Attribute  = UDBAttr_Stats::GetHealthAttribute();
	HealMod.ModifierOp = EGameplayModOp::Additive;

	// The actual heal amount is supplied by the GA:
	// SpecHandle.Data->SetSetByCallerMagnitude("SetByCaller.Healing", FinalHeal);
	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing"));
	HealMod.ModifierMagnitude = SBC;

	Modifiers.Add(HealMod);

	// NOTE (UE5.6+):
	// InheritableOwnedTagsContainer is deprecated. If you want the GE to grant tags
	// to the target or the source, configure a Gameplay Effect Component on the asset.
	// In the editor, open the GE asset (created from this class) and add:
	//   - "Granted Tags" (aka "UTargetTagsGameplayEffectComponent") for tags applied to the target, or
	//   - "Granted Application Tags"/"Source Tags" components depending on your need.
	// Then set tags there. No deprecated APIs used here.
}
