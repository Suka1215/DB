#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DBPlayerController.generated.h"

class UUserWidget;
class UDBW_GameHUD;
class UDBW_AbilityDebugPanel;
class ADBPlayerCharacter;

UCLASS()
class DAWNBLADE_API ADBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADBPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// Assign these in C++ constructor or (later) in a BP subclass if you make one
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> GameHUDClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> DebugPanelClass;

private:
	UPROPERTY() TObjectPtr<UUserWidget> GameHUDInstance = nullptr;
	UPROPERTY() TObjectPtr<UUserWidget> DebugPanelInstance = nullptr;

	void CreateOrShowHUD();
	void InitWidgetsForPawn(ADBPlayerCharacter* PC);
};
