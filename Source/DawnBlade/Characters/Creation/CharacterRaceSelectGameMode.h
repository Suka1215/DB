#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterRaceSelectGameMode.generated.h"

class ACharacterRaceLineup;
class URaceSelectWidget;

/**
 * GameMode for the Race Selection screen.
 * Spawns lineup of races and attaches the selection UI.
 */
UCLASS()
class DAWNBLADE_API ACharacterRaceSelectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACharacterRaceSelectGameMode();

protected:
	virtual void BeginPlay() override;

public:
	/** Class of the lineup actor (spawned dynamically at runtime) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Race Select")
	TSubclassOf<ACharacterRaceLineup> LineupClass;

	/** Class of the UI widget for race selection */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Race Select")
	TSubclassOf<URaceSelectWidget> RaceSelectWidgetClass;

	/** Reference to the spawned lineup actor */
	UPROPERTY(BlueprintReadOnly, Category="Race Select")
	TWeakObjectPtr<ACharacterRaceLineup> SpawnedLineup;

	/** Reference to the race selection widget */
	UPROPERTY(BlueprintReadOnly, Category="Race Select")
	TWeakObjectPtr<URaceSelectWidget> RaceSelectWidget;
};
