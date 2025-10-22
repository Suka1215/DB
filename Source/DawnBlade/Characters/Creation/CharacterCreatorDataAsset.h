// CharacterCreatorDataAsset.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimBlueprint.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "CharacterCreatorDataAsset.generated.h"

UCLASS(BlueprintType, Category = "Character Creator")
class DAWNBLADE_API UCharacterCreatorDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UCharacterCreatorDataAsset();

    // === Base MetaHuman Assets ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Assets")
    TSoftObjectPtr<USkeletalMesh> MaleBaseSkeleton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Assets")
    TSoftObjectPtr<USkeletalMesh> FemaleBaseSkeleton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Assets")
    TSoftClassPtr<UAnimInstance> CharacterAnimBlueprint;

    // === Category-Specific Presets ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hair Presets")
    TArray<FHairPreset> HairPresets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Presets")
    TArray<FEyePreset> EyePresets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nose Presets")
    TArray<FNosePreset> NosePresets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouth Presets")
    TArray<FMouthPreset> MouthPresets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body Presets")
    TArray<FBodyPreset> BodyPresets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Makeup Presets")
    TArray<FMakeupPreset> MakeupPresets;

    // NEW: Race presets (Hume, Altani, Eth, Talros, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Races")
    TArray<FRacePreset> RacePresets;

    // === Color Palettes ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hair Colors")
    TArray<FCharacterColorPreset> HairColors;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Colors")
    TArray<FCharacterColorPreset> EyeColors;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skin Colors")
    TArray<FCharacterColorPreset> SkinTones;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lip Colors")
    TArray<FCharacterColorPreset> LipColors;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Makeup Colors")
    TArray<FCharacterColorPreset> MakeupColors;

    // === MetaHuman Morph Target & Material Mappings ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MetaHuman Mappings")
    TMap<FString, FString> FacialMorphTargetMappings;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MetaHuman Mappings")
    TMap<FString, FString> BodyMorphTargetMappings;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MetaHuman Mappings")
    TMap<FString, FString> MaterialParameterMappings;
    
    // === Utility Functions ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FHairPreset> GetHairPresets() const { return HairPresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FEyePreset> GetEyePresets() const { return EyePresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FNosePreset> GetNosePresets() const { return NosePresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FMouthPreset> GetMouthPresets() const { return MouthPresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FBodyPreset> GetBodyPresets() const { return BodyPresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FRacePreset> GetRacePresets() const { return RacePresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FMakeupPreset> GetMakeupPresets() const { return MakeupPresets; }

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FCharacterColorPreset> GetColorsForCategory(ECharacterFeatureCategory Category) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    bool IsPresetUnlocked(ECharacterFeatureCategory Category, int32 PresetIndex, int32 PlayerLevel, bool bHasPremium) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    bool IsColorUnlocked(ECharacterFeatureCategory Category, int32 ColorIndex, int32 PlayerLevel, bool bHasPremium) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FString GetMorphTargetName(const FString& PropertyName) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FString GetMaterialParameterName(const FString& PropertyName) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TSoftObjectPtr<USkeletalMesh> GetBaseSkeletonForGender(ECharacterGender Gender) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    int32 GetPresetCount(ECharacterFeatureCategory Category) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    int32 GetColorCount(ECharacterFeatureCategory Category) const;

protected:
    // === Initialization Helpers ===
    void InitializeDefaultColors();
    void InitializeDefaultMorphTargetMappings();
    void InitializeDefaultMaterialMappings();

public:
    // === Validation ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator|Validation")
    bool ValidateHairPreset(int32 PresetIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator|Validation")
    bool ValidateEyePreset(int32 PresetIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator|Validation")
    bool ValidateColorIndex(ECharacterFeatureCategory Category, int32 ColorIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Character Creator|Validation")
    bool ValidateAppearanceData(const FCharacterAppearanceData& AppearanceData, int32 PlayerLevel, bool bHasPremium) const;

#if WITH_EDITOR
    // === Editor Helpers ===
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    UFUNCTION(CallInEditor, Category = "Character Creator|Editor")
    void PopulateDefaultHairPresets();

    UFUNCTION(CallInEditor, Category = "Character Creator|Editor")
    void PopulateDefaultEyePresets();

    UFUNCTION(CallInEditor, Category = "Character Creator|Editor")
    void PopulateDefaultColorPalettes();

    UFUNCTION(CallInEditor, Category = "Character Creator|Editor")
    void ValidateAllPresets();
#endif
};
