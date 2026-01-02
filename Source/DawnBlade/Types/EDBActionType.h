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

UENUM(BlueprintType)
enum class EDBSpellEffect : uint8
{
	None UMETA(DisplayName = "None"),
	Attack UMETA(DisplayName = "Attack"),
	Healing UMETA(DisplayName = "Healing"),
	Support UMETA(DisplayName = "Support"),
};

UENUM(BlueprintType)
enum class EDBSpellType : uint8
{
	None UMETA(DisplayName = "None"),

	// ⚪ Radiance Magic — The light of creation.
	// Magic of healing, protection, and renewal, drawing upon aether and sacred energy.
	// Practiced by Clerics, Paladins, and Lightbearers to restore life and banish darkness.
	Radiance UMETA(DisplayName = "Radiance"),

	// ⚫ Obsidian Magic — The art of entropy and destruction.
	// Manipulates shadow, void, and forbidden power to devastate enemies or drain their vitality.
	// Favored by Warlocks, Dark Knights, and Arcanists.
	Obsidian UMETA(DisplayName = "Obsidian"),

	// 🌊🔥❄️ Elemental Magic — Command of the primal forces.
	// Channels the raw powers of flame, frost, wind, earth, thunder, and tide.
	// Used by Elementalists and Rune Sages to unleash nature’s fury.
	Elemental UMETA(DisplayName = "Elemental"),

	// ✨ Enhancing Magic — The art of empowerment.
	// Grants boons, shields, and blessings that fortify allies or amplify their abilities.
	// Practiced by Wardens, Scholars, and Enchanters.
	Enhancing UMETA(DisplayName = "Enhancing"),

	// 🕊️ Divine Magic — Judgment of the higher realms.
	// Holy incantations that smite darkness and channel celestial authority.
	// Wielded by Templars, Crusaders, and Oracles.
	Divine UMETA(DisplayName = "Divine"),

	// 🩸 Crimson Magic — The language of curses and affliction.
	// Specializes in weakening foes through poison, fear, bleeding, or corruption.
	// Mastered by Hexblades and Blood Mages.
	Crimson UMETA(DisplayName = "Crimson"),

	// 💙 Azure Magic — Echoes of the living world.
	// Adaptive arts that imitate monster abilities by absorbing their essence.
	// Practiced by Echomancers and Soulbinders who learn from every battle.
	Azure UMETA(DisplayName = "Azure"),

	// 🔮 Celestial Magic — Pacts with the stars.
	// Invokes beings from the astral and divine planes to fight beside the caster.
	// Harnessed by Summoners, Pactsworn, and Ascendants.
	Celestial UMETA(DisplayName = "Celestial"),
};

