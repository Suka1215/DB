#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CharacterPresetSaveGame.generated.h"

/**
 * SaveGame object used for storing character creator presets
 * Stores version and serialized FCharacterAppearanceData
 */
UCLASS()
class DAWNBLADE_API UCharacterPresetSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Version string for backwards compatibility */
	UPROPERTY(VisibleAnywhere, Category = "Character Preset")
	FString Version;

	/** Serialized appearance data (stored as JSON for portability) */
	UPROPERTY(VisibleAnywhere, Category = "Character Preset")
	FString AppearanceJson;
};