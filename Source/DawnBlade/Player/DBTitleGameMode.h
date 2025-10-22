#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GUI/Menus/DBW_TitleScreen.h"
#include "DBTitleGameMode.generated.h"

class UDBTitleScreen;

UCLASS()
class DAWNBLADE_API ADBTitleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADBTitleGameMode();

protected:
	virtual void BeginPlay() override;

	// Widget class for title screen - exposed to Blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UDBW_TitleScreen> TitleScreenWidgetClass;

private:
	// Instance of the title screen widget
	UPROPERTY()
	UDBW_TitleScreen* TitleScreenWidget;

	void CreateTitleScreenWidget();
};