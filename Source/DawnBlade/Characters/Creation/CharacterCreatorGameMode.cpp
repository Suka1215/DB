// CharacterCreatorGameMode.cpp
// AAA-ready implementation for MetaHuman Character Creator

#include "Characters/Creation/CharacterCreatorGameMode.h"
#include "Characters/Creation/CharacterPresetSaveGame.h"
#include "GUI/MetaHumanStyleCharacterCreator.h"

#include "JsonObjectConverter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Static version string for preset saves
const FString ACharacterCreatorGameMode::SaveVersion = TEXT("1.0.0");

ACharacterCreatorGameMode::ACharacterCreatorGameMode()
{
    // No pawn needed for menu
    DefaultPawnClass = nullptr;
    PlayerControllerClass = APlayerController::StaticClass();
    HUDClass = nullptr;
    PlayerStateClass = nullptr;

    CharacterCreatorWidgetClass = nullptr;
}

void ACharacterCreatorGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!DefaultCreatorData)
    {
        UE_LOG(LogTemp, Warning, TEXT("No DefaultCreatorData assigned in GameMode!"));
    }

    // Spawn UI
    CreateCharacterCreatorWidget();
}

void ACharacterCreatorGameMode::CreateCharacterCreatorWidget()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("No PlayerController found!"));
        return;
    }

    if (!CharacterCreatorWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterCreatorWidgetClass is not set!"));
        return;
    }

    CharacterCreatorWidget = CreateWidget<UMetaHumanCharacterCreatorWidget>(PC, CharacterCreatorWidgetClass);
    if (CharacterCreatorWidget)
    {
        CharacterCreatorWidget->AddToViewport();

        // Set input mode to UI only
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(CharacterCreatorWidget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PC->SetInputMode(InputModeData);
        PC->SetShowMouseCursor(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create CharacterCreator widget"));
    }
}

void ACharacterCreatorGameMode::SaveCharacterPreset(const FString& PresetName, const FCharacterAppearanceData& CustomizationData)
{
    if (PresetName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveCharacterPreset failed: PresetName is empty"));
        return;
    }

    const FString SlotName = GetSaveSlotName(PresetName);

    // Create SaveGame object dynamically
    UCharacterPresetSaveGame* SaveGameObj = Cast<UCharacterPresetSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UCharacterPresetSaveGame::StaticClass())
    );
    if (!SaveGameObj)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create SaveGame object for preset: %s"), *PresetName);
        return;
    }

    // Convert FCharacterAppearanceData to JSON
    FString JsonString;
    if (!FJsonObjectConverter::UStructToJsonObjectString(CustomizationData, JsonString))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to serialize appearance data for preset: %s"), *PresetName);
        return;
    }

    SaveGameObj->Version = SaveVersion;
    SaveGameObj->AppearanceJson = JsonString;

    if (!UGameplayStatics::SaveGameToSlot(SaveGameObj, SlotName, 0))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save preset: %s"), *PresetName);
    }
}

bool ACharacterCreatorGameMode::LoadCharacterPreset(const FString& PresetName, FCharacterAppearanceData& OutCustomizationData)
{
    if (PresetName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadCharacterPreset failed: PresetName is empty"));
        return false;
    }

    const FString SlotName = GetSaveSlotName(PresetName);

    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        return false;
    }

    UCharacterPresetSaveGame* LoadedGame = Cast<UCharacterPresetSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SlotName, 0)
    );
    if (!LoadedGame)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save slot for preset: %s"), *PresetName);
        return false;
    }

    if (LoadedGame->AppearanceJson.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No appearance data found in preset: %s"), *PresetName);
        return false;
    }

    if (!FJsonObjectConverter::JsonObjectStringToUStruct(LoadedGame->AppearanceJson, &OutCustomizationData, 0, 0))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize appearance data for preset: %s"), *PresetName);
        return false;
    }

    return true;
}

TArray<FString> ACharacterCreatorGameMode::GetSavedPresetNames()
{
    TArray<FString> PresetNames;
    // TODO: Implement database integration for preset enumeration
    return PresetNames;
}

FString ACharacterCreatorGameMode::GetSaveSlotName(const FString& PresetName) const
{
    return FString::Printf(TEXT("Preset_%s"), *PresetName);
}