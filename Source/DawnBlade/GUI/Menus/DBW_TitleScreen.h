// ========================================
// DBTitleScreenWidget.h - Main Title Screen (Like FF14)
// ========================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Core/TransitionManager.h"
#include "DBW_TitleScreen.generated.h"

class UButton;
class UTextBlock;
class UImage;
class USoundBase;

UCLASS()
class DAWNBLADE_API UDBW_TitleScreen : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// UI Components
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* versionText;

	// Audio
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* TitleMusic;

	// Transition settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	FTransitionSettings TransitionSettings;

private:
	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnCreditsClicked();

	UFUNCTION()
	void OnQuitClicked();

	void StartBackgroundMusic();
};