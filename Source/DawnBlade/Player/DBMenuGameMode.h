// DBMenuGameMode.h - New class for menu/login screen
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DBMenuGameMode.generated.h"

class UDBW_MainMenu;

UCLASS()
class DAWNBLADE_API ADBMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADBMenuGameMode();

protected:
	virtual void BeginPlay() override;

	// Widget class for login screen - NOW EXPOSED TO BLUEPRINT
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UDBW_MainMenu> MainMenuWidgetClass;

private:
	UPROPERTY()
	UDBW_MainMenu* MainMenuWidget;

	void CreateMainMenuWidget();
};