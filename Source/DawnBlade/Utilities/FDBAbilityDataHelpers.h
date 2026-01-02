// DBAbilityDataHelpers.h
#pragma once

#include "CoreMinimal.h"
#include "Types/FDBAbilityData.h"

/**
 * Static utility functions for working with FDBAbilityData
 * Use for validation, tooltips, and UI display
 */
class DAWNBLADE_API FDBAbilityDataHelpers
{
public:
	/** Validate that ability data follows classification rules */
	static void ValidateAbilityData(const FDBAbilityData* Row);
    
	/** Generate formatted tooltip text for UI */
	static FText GetAbilityTooltip(const FDBAbilityData* AbilityData);
    
	/** Get spell type as display text (e.g., "Radiance", "Obsidian") */
	static FText GetSpellTypeDisplayName(EDBSpellType SpellType);
    
	/** Get spell effect as display text (e.g., "Attack", "Healing") */
	static FText GetSpellEffectDisplayName(EDBSpellEffect SpellEffect);
    
	/** Get action type as display text (e.g., "Spell", "Job Ability") */
	static FText GetActionTypeDisplayName(EDBActionType ActionType);
};