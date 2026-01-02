// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/FDBAbilityDataHelpers.h"

void FDBAbilityDataHelpers::ValidateAbilityData(const FDBAbilityData* Row)
{
    if (!Row) return;

    // Rule: Only Spells should have SpellType and SpellEffect
    if (Row->ActionType == EDBActionType::Spell)
    {
        if (Row->SpellType == EDBSpellType::None)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠ SPELL %s has no SpellType!"), *Row->AbilityName.ToString());
        }
        if (Row->SpellEffect == EDBSpellEffect::None)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠ SPELL %s has no SpellEffect!"), *Row->AbilityName.ToString());
        }
    }
    else // JobAbility, Passive, etc.
    {
        if (Row->SpellType != EDBSpellType::None)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠ %s is not a Spell but has SpellType set!"), 
                *Row->AbilityName.ToString());
        }
        if (Row->SpellEffect != EDBSpellEffect::None)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠ %s is not a Spell but has SpellEffect set!"), 
                *Row->AbilityName.ToString());
        }
    }
}

FText FDBAbilityDataHelpers::GetAbilityTooltip(const FDBAbilityData* AbilityData)
{
    if (!AbilityData) return FText::GetEmpty();

    // Start with name and description
    FString TooltipText = FString::Printf(TEXT("%s\n\n%s"),
        *AbilityData->AbilityName.ToString(),
        *AbilityData->Description.ToString()
    );

    // Add type classification
    if (AbilityData->ActionType == EDBActionType::Spell)
    {
        FString SpellTypeStr = GetSpellTypeDisplayName(AbilityData->SpellType).ToString();
        FString SpellEffectStr = GetSpellEffectDisplayName(AbilityData->SpellEffect).ToString();
        
        TooltipText += FString::Printf(TEXT("\n\nType: %s Spell\nEffect: %s"),
            *SpellTypeStr, *SpellEffectStr);
    }
    else
    {
        FString ActionTypeStr = GetActionTypeDisplayName(AbilityData->ActionType).ToString();
        TooltipText += FString::Printf(TEXT("\n\nType: %s"), *ActionTypeStr);
    }

    // Add costs
    if (AbilityData->MPCost > 0)
        TooltipText += FString::Printf(TEXT("\nMP Cost: %.0f"), AbilityData->MPCost);
    if (AbilityData->TPCost > 0)
        TooltipText += FString::Printf(TEXT("\nTP Cost: %.0f"), AbilityData->TPCost);
    if (AbilityData->HPCost > 0)
        TooltipText += FString::Printf(TEXT("\nHP Cost: %.0f"), AbilityData->HPCost);
    
    // Add timing info
    if (AbilityData->CastTime > 0)
        TooltipText += FString::Printf(TEXT("\nCast Time: %.1fs"), AbilityData->CastTime);
    if (AbilityData->Cooldown > 0)
        TooltipText += FString::Printf(TEXT("\nCooldown: %.1fs"), AbilityData->Cooldown);
    if (AbilityData->Duration > 0)
        TooltipText += FString::Printf(TEXT("\nDuration: %.1fs"), AbilityData->Duration);
    
    // Add range
    if (AbilityData->Range > 0)
        TooltipText += FString::Printf(TEXT("\nRange: %.0f"), AbilityData->Range);
    if (AbilityData->AoeRadius > 0)
        TooltipText += FString::Printf(TEXT("\nRadius: %.0f"), AbilityData->AoeRadius);

    return FText::FromString(TooltipText);
}

FText FDBAbilityDataHelpers::GetSpellTypeDisplayName(EDBSpellType SpellType)
{
    switch (SpellType)
    {
        case EDBSpellType::None:      return FText::FromString(TEXT("None"));
        case EDBSpellType::Radiance:  return FText::FromString(TEXT("⚪ Radiance"));
        case EDBSpellType::Obsidian:  return FText::FromString(TEXT("⚫ Obsidian"));
        case EDBSpellType::Elemental: return FText::FromString(TEXT("🌊 Elemental"));
        case EDBSpellType::Enhancing: return FText::FromString(TEXT("✨ Enhancing"));
        case EDBSpellType::Divine:    return FText::FromString(TEXT("🕊️ Divine"));
        case EDBSpellType::Crimson:   return FText::FromString(TEXT("🩸 Crimson"));
        case EDBSpellType::Azure:     return FText::FromString(TEXT("💙 Azure"));
        case EDBSpellType::Celestial: return FText::FromString(TEXT("🔮 Celestial"));
        default: return FText::FromString(TEXT("Unknown"));
    }
}

FText FDBAbilityDataHelpers::GetSpellEffectDisplayName(EDBSpellEffect SpellEffect)
{
    switch (SpellEffect)
    {
        case EDBSpellEffect::None:    return FText::FromString(TEXT("None"));
        case EDBSpellEffect::Attack:  return FText::FromString(TEXT("Attack"));
        case EDBSpellEffect::Healing: return FText::FromString(TEXT("Healing"));
        case EDBSpellEffect::Support: return FText::FromString(TEXT("Support"));
        default: return FText::FromString(TEXT("Unknown"));
    }
}

FText FDBAbilityDataHelpers::GetActionTypeDisplayName(EDBActionType ActionType)
{
    switch (ActionType)
    {
        case EDBActionType::Spell:        return FText::FromString(TEXT("Spell"));
        case EDBActionType::JobAbility:   return FText::FromString(TEXT("Job Ability"));
        case EDBActionType::Ninjutsu:     return FText::FromString(TEXT("Ninjutsu"));
        case EDBActionType::WeaponSkill:  return FText::FromString(TEXT("Weapon Skill"));
        case EDBActionType::Passive:      return FText::FromString(TEXT("Passive/Trait"));
        case EDBActionType::Item:         return FText::FromString(TEXT("Item"));
        default: return FText::FromString(TEXT("Unknown"));
    }
}










