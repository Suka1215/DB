#pragma once

UENUM(BlueprintType)
enum class EDBActionType : uint8
{
	Spell UMETA(DisplayName = "Spell"),
	JobAbility UMETA(DisplayName = "Job Ability"),
	Ninjutsu UMETA(DisplayName = "Ninjutsu"),
	WeaponSkill UMETA(DisplayName = "Weapon Skill"),
	Passive UMETA(DisplayName = "Passive/Trait"),
	Item UMETA(DisplayName = "Item"),
};
