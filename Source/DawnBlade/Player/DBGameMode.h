// DBGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DBGameMode.generated.h"

/**
 * GameMode for actual gameplay (not menu/login)
 * Uses your custom player classes and game systems
 */
UCLASS()
class DAWNBLADE_API ADBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADBGameMode();

protected:
	virtual void BeginPlay() override;

	// Optional: Add any gameplay-specific functionality here
	// For example, respawn logic, game state management, etc.
};