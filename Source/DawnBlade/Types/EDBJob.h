#pragma once

UENUM(BlueprintType)
enum class EDBJob : uint8
{
	None UMETA(DisplayName = "None"),
	LightMage UMETA(DisplayName = "Light Mage"),
	Warrior UMETA(DisplayName = "Warrior"),
	Paladin UMETA(DisplayName = "Paladin"),
};
