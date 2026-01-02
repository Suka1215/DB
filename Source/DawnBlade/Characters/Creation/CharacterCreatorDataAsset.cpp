#include "Characters/Creation/CharacterCreatorDataAsset.h"
#include "UObject/ConstructorHelpers.h"
#include "GroomAsset.h"
#include "Engine/Engine.h"

UCharacterCreatorDataAsset::UCharacterCreatorDataAsset()
{
    // Initialize with default values
    InitializeDefaultColors();
    InitializeDefaultMorphTargetMappings();
    InitializeDefaultMaterialMappings();

#if WITH_EDITORONLY_DATA
    PopulateDefaultHairPresets();
    PopulateDefaultEyePresets();
#endif
}

// ======================================================
// === Colors Per Category
// ======================================================
void UCharacterCreatorDataAsset::InitializeDefaultColors()
{
    // --- Hair Colors ---
    if (HairColors.IsEmpty())
    {
        HairColors = {
            {FLinearColor::Black, TEXT("Black")},
            {FLinearColor(0.1f,0.05f,0.02f), TEXT("Dark Brown")},
            {FLinearColor(0.35f,0.2f,0.1f), TEXT("Brown")},
            {FLinearColor(0.9f,0.85f,0.6f), TEXT("Blonde")},
            {FLinearColor(0.7f,0.25f,0.15f), TEXT("Red")},
            {FLinearColor(0.95f,0.95f,0.95f), TEXT("Silver")},
            {FLinearColor(0.2f,0.2f,0.8f), TEXT("Blue"), 10, true},
            {FLinearColor(0.6f,0.2f,0.7f), TEXT("Purple"), 12, true}
        };
    }

    // --- Eye Colors ---
    if (EyeColors.IsEmpty())
    {
        EyeColors = {
            {FLinearColor(0.2f,0.4f,0.8f), TEXT("Blue")},
            {FLinearColor(0.3f,0.65f,0.2f), TEXT("Green")},
            {FLinearColor(0.25f,0.1f,0.05f), TEXT("Brown")},
            {FLinearColor(0.75f,0.75f,0.8f), TEXT("Gray")},
            {FLinearColor(0.55f,0.45f,0.25f), TEXT("Hazel")}
        };
    }

    // --- Skin Tones (~40 swatches) ---
    if (SkinTones.IsEmpty())
    {
        for (int32 i = 0; i < 40; i++)
        {
            float t = i / 39.0f;
            FLinearColor Skin = FLinearColor::LerpUsingHSV(
                FLinearColor(0.98f, 0.87f, 0.73f), // lightest
                FLinearColor(0.20f, 0.12f, 0.08f), // darkest
                t
            );

            SkinTones.Add({ Skin, FString::Printf(TEXT("Skin Tone %d"), i + 1) });
        }
    }

    // --- Lip Colors ---
    if (LipColors.IsEmpty())
    {
        LipColors = {
            {FLinearColor(0.85f,0.6f,0.55f), TEXT("Natural")},
            {FLinearColor(0.8f,0.2f,0.2f), TEXT("Red")},
            {FLinearColor(0.6f,0.2f,0.5f), TEXT("Plum")},
            {FLinearColor(0.95f,0.7f,0.75f), TEXT("Pink")},
            {FLinearColor(0.3f,0.05f,0.05f), TEXT("Deep Wine"), 5, true}
        };
    }

    // --- Makeup Colors ---
    if (MakeupColors.IsEmpty())
    {
        MakeupColors = {
            {FLinearColor(0.85f,0.6f,0.45f), TEXT("Natural")},
            {FLinearColor(0.6f,0.4f,0.5f), TEXT("Rose Gold")},
            {FLinearColor(0.2f,0.2f,0.2f), TEXT("Smokey Black")},
            {FLinearColor(0.9f,0.3f,0.3f), TEXT("Crimson Red")}
        };
    }
}

// ======================================================
// === Morph Target and Material Mappings
// ======================================================
void UCharacterCreatorDataAsset::InitializeDefaultMorphTargetMappings()
{
    if (FacialMorphTargetMappings.IsEmpty())
    {
        FacialMorphTargetMappings.Add(TEXT("EyeSize"), TEXT("CTRL_C_Eye_Big"));
        FacialMorphTargetMappings.Add(TEXT("NoseWidth"), TEXT("CTRL_C_Nose_Wide"));
        FacialMorphTargetMappings.Add(TEXT("MouthWidth"), TEXT("CTRL_C_Mouth_Wide"));
        FacialMorphTargetMappings.Add(TEXT("LipThickness"), TEXT("CTRL_C_Lip_Up_Thick"));
    }

    if (BodyMorphTargetMappings.IsEmpty())
    {
        BodyMorphTargetMappings.Add(TEXT("Height"), TEXT("Height"));
        BodyMorphTargetMappings.Add(TEXT("Weight"), TEXT("Weight"));
        BodyMorphTargetMappings.Add(TEXT("Muscle"), TEXT("Muscle"));
    }
}

void UCharacterCreatorDataAsset::InitializeDefaultMaterialMappings()
{
    if (MaterialParameterMappings.IsEmpty())
    {
        MaterialParameterMappings.Add(TEXT("HairColor"), TEXT("Hair Color"));
        MaterialParameterMappings.Add(TEXT("EyeColor"), TEXT("Eye Color"));
        MaterialParameterMappings.Add(TEXT("SkinTone"), TEXT("Skin Tone"));
        MaterialParameterMappings.Add(TEXT("LipColor"), TEXT("Lip Color"));
        MaterialParameterMappings.Add(TEXT("MakeupColor"), TEXT("Makeup Color"));
    }
}

// ======================================================
// === Utility Functions
// ======================================================
TArray<FCharacterColorPreset> UCharacterCreatorDataAsset::GetColorsForCategory(ECharacterFeatureCategory Category) const
{
    switch (Category)
    {
        case ECharacterFeatureCategory::Hair:   return HairColors;
        case ECharacterFeatureCategory::Eyes:   return EyeColors;
        case ECharacterFeatureCategory::Nose:
        case ECharacterFeatureCategory::Body:   return SkinTones;
        case ECharacterFeatureCategory::Mouth:  return LipColors;
        case ECharacterFeatureCategory::Makeup: return MakeupColors;
        default: return {};
    }
}

bool UCharacterCreatorDataAsset::IsPresetUnlocked(ECharacterFeatureCategory Category, int32 PresetIndex, int32 PlayerLevel, bool bHasPremium) const
{
    UE_LOG(LogTemp, Log, TEXT("IsPresetUnlocked(Category=%d, PresetIndex=%d, PlayerLevel=%d, Premium=%d)"),
        (int32)Category, PresetIndex, PlayerLevel, bHasPremium);
    return true;
}

bool UCharacterCreatorDataAsset::IsColorUnlocked(ECharacterFeatureCategory Category, int32 ColorIndex, int32 PlayerLevel, bool bHasPremium) const
{
    UE_LOG(LogTemp, Log, TEXT("IsColorUnlocked(Category=%d, ColorIndex=%d, PlayerLevel=%d, Premium=%d)"),
        (int32)Category, ColorIndex, PlayerLevel, bHasPremium);
    return true;
}

FString UCharacterCreatorDataAsset::GetMorphTargetName(const FString& PropertyName) const
{
    if (const FString* Found = FacialMorphTargetMappings.Find(PropertyName))
    {
        return *Found;
    }
    return FString::Printf(TEXT("Morph_%s"), *PropertyName);
}

FString UCharacterCreatorDataAsset::GetMaterialParameterName(const FString& PropertyName) const
{
    if (const FString* Found = MaterialParameterMappings.Find(PropertyName))
    {
        return *Found;
    }
    return FString::Printf(TEXT("Param_%s"), *PropertyName);
}

TSoftObjectPtr<USkeletalMesh> UCharacterCreatorDataAsset::GetBaseSkeletonForGender(ECharacterGender Gender) const
{
    return (Gender == ECharacterGender::Male) ? MaleBaseSkeleton : FemaleBaseSkeleton;
}

int32 UCharacterCreatorDataAsset::GetPresetCount(ECharacterFeatureCategory Category) const
{
    switch (Category)
    {
        case ECharacterFeatureCategory::Hair:   return HairPresets.Num();
        case ECharacterFeatureCategory::Eyes:   return EyePresets.Num();
        case ECharacterFeatureCategory::Nose:   return NosePresets.Num();
        case ECharacterFeatureCategory::Mouth:  return MouthPresets.Num();
        case ECharacterFeatureCategory::Body:   return BodyPresets.Num();
        case ECharacterFeatureCategory::Makeup: return MakeupPresets.Num();
        default: return 0;
    }
}

int32 UCharacterCreatorDataAsset::GetColorCount(ECharacterFeatureCategory Category) const
{
    return GetColorsForCategory(Category).Num();
}

bool UCharacterCreatorDataAsset::ValidateHairPreset(int32 PresetIndex) const
{
    return HairPresets.IsValidIndex(PresetIndex);
}

bool UCharacterCreatorDataAsset::ValidateEyePreset(int32 PresetIndex) const
{
    return EyePresets.IsValidIndex(PresetIndex);
}

bool UCharacterCreatorDataAsset::ValidateColorIndex(ECharacterFeatureCategory Category, int32 ColorIndex) const
{
    return GetColorsForCategory(Category).IsValidIndex(ColorIndex);
}

bool UCharacterCreatorDataAsset::ValidateAppearanceData(const FCharacterAppearanceData& AppearanceData, int32 PlayerLevel, bool bHasPremium) const
{
    return true; // TODO: expand later
}

// ======================================================
// === Editor Helpers
// ======================================================
#if WITH_EDITOR
void UCharacterCreatorDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (PropertyChangedEvent.Property)
    {
        UE_LOG(LogTemp, Log, TEXT("Property '%s' changed"), *PropertyChangedEvent.Property->GetName());
    }
}

void UCharacterCreatorDataAsset::PopulateDefaultHairPresets()
{
    HairPresets.Empty();

    FHairPreset Preset;
    Preset.DisplayName = FText::FromString("Swept Up");
    Preset.HairLength = 0.4f;
    Preset.HairVolume = 0.5f;
    Preset.UnlockLevel = 1;
    Preset.bRequiresPremium = false;
    Preset.HairAttachment.HairMesh = nullptr; // assign later
    Preset.HairAttachment.HairMaterial = nullptr; // assign later
    
    HairPresets.Add(Preset);
}

void UCharacterCreatorDataAsset::PopulateDefaultEyePresets()
{
    EyePresets.Empty();

    FEyePreset Preset;
    Preset.DisplayName = FText::FromString("Standard Eye Shape");
    Preset.EyeSize = 0.5f;
    Preset.UnlockLevel = 1;

    EyePresets.Add(Preset);
}

void UCharacterCreatorDataAsset::PopulateDefaultColorPalettes()
{
    InitializeDefaultColors();
}

void UCharacterCreatorDataAsset::ValidateAllPresets()
{
    int32 Issues = 0;

    for (int32 i = 0; i < HairPresets.Num(); ++i)
        if (HairPresets[i].DisplayName.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hair preset %d missing name"), i);
            Issues++;
        }

    for (int32 i = 0; i < EyePresets.Num(); ++i)
        if (EyePresets[i].DisplayName.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Eye preset %d missing name"), i);
            Issues++;
        }

    if (Issues == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("All presets validated successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%d preset issues found"), Issues);
    }
}
#endif
