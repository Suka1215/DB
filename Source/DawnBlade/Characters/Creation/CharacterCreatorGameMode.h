// CharacterCreatorGameMode.h
// AAA-ready GameMode for MetaHuman Character Creator

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "CharacterCreatorDataAsset.h"
#include "CharacterCreatorGameMode.generated.h"

// Forward declaration
class UCharacterPresetSaveGame;
class UMetaHumanCharacterCreatorWidget;

/**
 * GameMode responsible for orchestrating the Character Creator session.
 * - Provides save/load for appearance presets
 * - Holds reference to default DataAsset and CreatorPawn
 * - Intended for both offline prototyping and online MMO integration
 */
UCLASS()
class DAWNBLADE_API ACharacterCreatorGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACharacterCreatorGameMode();

protected:
    virtual void BeginPlay() override;

    /** Spawn and setup the character creator UI */
    void CreateCharacterCreatorWidget();

    /** Widget instance */
    UPROPERTY()
    UMetaHumanCharacterCreatorWidget* CharacterCreatorWidget;

    /** Class to spawn for UI */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
    TSubclassOf<UMetaHumanCharacterCreatorWidget> CharacterCreatorWidgetClass;

public:
    /** Pawn to spawn in the character creator scene */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Creator")
    TSubclassOf<class APawn> CharacterCreatorPawn;

    /** Default DataAsset containing all preset data */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Creator")
    UCharacterCreatorDataAsset* DefaultCreatorData;

    // === Save/Load Presets ===

    /** Save a character preset locally (offline) or send to DB (online) */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SaveCharacterPreset(const FString& PresetName, const FCharacterAppearanceData& CustomizationData);

    /** Load a character preset from local save (offline) or DB (online) */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    bool LoadCharacterPreset(const FString& PresetName, FCharacterAppearanceData& OutCustomizationData);

    /** Return list of locally saved preset names (offline) */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FString> GetSavedPresetNames();

private:
    /** Internal helper to build the SaveGame slot name */
    FString GetSaveSlotName(const FString& PresetName) const;

    /** Internal version key for backwards compatibility */
    static const FString SaveVersion;
};
