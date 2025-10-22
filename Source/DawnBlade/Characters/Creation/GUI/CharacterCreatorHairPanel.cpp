// CharacterCreatorHairPanel.cpp

#include "CharacterCreatorHairPanel.h"
#include "Characters/Creation/CharacterCreatorComponent.h"
#include "Characters/Creation/CharacterCreatorDataAsset.h"
#include "FeaturePresetButton.h"
#include "Components/UniformGridPanel.h"
#include "Components/ScrollBox.h"

UCharacterCreatorHairPanel::UCharacterCreatorHairPanel(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UCharacterCreatorHairPanel::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UCharacterCreatorHairPanel::InitializePanel(UCharacterCreatorComponent* InCreatorComponent)
{
    CharacterCreatorComponent = InCreatorComponent;

    if (CharacterCreatorComponent)
    {
        PopulateHairPresets();
    }
}

void UCharacterCreatorHairPanel::PopulateHairPresets()
{
    if (!CharacterCreatorComponent || !CharacterCreatorComponent->CreatorDataAsset || !HairGrid)
        return;

    HairGrid->ClearChildren();
    HairPresetButtons.Empty();

    const TArray<FHairPreset>& Presets = CharacterCreatorComponent->CreatorDataAsset->GetHairPresets();

    for (int32 i = 0; i < Presets.Num(); ++i)
    {
        if (!HairPresetButtonClass) continue;

        UFeaturePresetButton* PresetButton = CreateWidget<UFeaturePresetButton>(this, HairPresetButtonClass);
        if (PresetButton)
        {
            PresetButton->SetPresetData(
                Presets[i].DisplayName,
                Presets[i].PreviewThumbnail,
                Presets[i].UnlockLevel,
                Presets[i].bRequiresPremium,
                i
            );

            PresetButton->OnFeaturePresetSelected.AddDynamic(this, &UCharacterCreatorHairPanel::OnHairPresetSelected);

            int32 Row = i / 3;
            int32 Column = i % 3;
            HairGrid->AddChildToUniformGrid(PresetButton, Row, Column);

            HairPresetButtons.Add(PresetButton);
        }
    }
}

void UCharacterCreatorHairPanel::OnHairPresetSelected(int32 PresetIndex)
{
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->ApplySlotMesh(ECharacterMeshSlot::Hair, PresetIndex);
    }

    // Update button selection
    for (int32 i = 0; i < HairPresetButtons.Num(); ++i)
    {
        if (HairPresetButtons[i])
        {
            HairPresetButtons[i]->SetPresetSelected(i == PresetIndex);
        }
    }
}
