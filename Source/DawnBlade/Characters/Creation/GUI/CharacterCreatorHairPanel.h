// CharacterCreatorHairPanel.h
#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "CharacterCreatorHairPanel.generated.h"

class UCharacterCreatorComponent;
class UFeaturePresetButton;
class UUniformGridPanel;
class UScrollBox;

/**
 * Hair panel widget for displaying all hair presets
 */
UCLASS()
class DAWNBLADE_API UCharacterCreatorHairPanel : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UCharacterCreatorHairPanel(const FObjectInitializer& ObjectInitializer);

	// Setup with a character creator component
	UFUNCTION(BlueprintCallable, Category = "Character Creator")
	void InitializePanel(UCharacterCreatorComponent* InCreatorComponent);

protected:
	virtual void NativeOnInitialized() override;

	// Callback when a preset is selected
	UFUNCTION()
	void OnHairPresetSelected(int32 PresetIndex);

	// Populate all available presets
	void PopulateHairPresets();

private:
	// The owning component that applies changes
	UPROPERTY()
	UCharacterCreatorComponent* CharacterCreatorComponent;

	// Grid container for preset buttons
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* HairGrid;

	// Scroll box containing grid
	UPROPERTY(meta = (BindWidget))
	UScrollBox* HairScrollBox;

	// Class for preset buttons
	UPROPERTY(EditAnywhere, Category = "Character Creator")
	TSubclassOf<UFeaturePresetButton> HairPresetButtonClass;

	// Store created buttons
	UPROPERTY()
	TArray<UFeaturePresetButton*> HairPresetButtons;
};
