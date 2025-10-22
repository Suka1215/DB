// CharacterCreatorTypes.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Engine/SkeletalMesh.h"
#include "GroomAsset.h"
#include "GroomBindingAsset.h"
#include "CharacterCreatorTypes.generated.h"

//
// === ENUMERATIONS ===
//

UENUM(BlueprintType)
enum class ECharacterRace : uint8
{
    Hume      UMETA(DisplayName = "Hume"),
    Altani    UMETA(DisplayName = "Altani"),
    Eth       UMETA(DisplayName = "Eth"),
    Talros    UMETA(DisplayName = "Talros"),
    // Extend later with new races...
    Custom    UMETA(DisplayName = "Custom")
};

// Feature categories used by UI and DataAsset
UENUM(BlueprintType)
enum class ECharacterFeatureCategory : uint8
{
    Hair   UMETA(DisplayName = "Hair"),
    Eyes   UMETA(DisplayName = "Eyes"),
    Nose   UMETA(DisplayName = "Nose"),
    Mouth  UMETA(DisplayName = "Mouth"),
    Body   UMETA(DisplayName = "Body"),
    Makeup UMETA(DisplayName = "Makeup")
};

// Gender for base bodies
UENUM(BlueprintType)
enum class ECharacterGender : uint8
{
    Male   UMETA(DisplayName = "Male"),
    Female UMETA(DisplayName = "Female")
};

// Skeletal mesh slots for MetaHuman modular characters
UENUM(BlueprintType)
enum class ECharacterMeshSlot : uint8
{
    Head  UMETA(DisplayName = "Head"),
    Body  UMETA(DisplayName = "Body"),
    Hands UMETA(DisplayName = "Hands"),
    Legs  UMETA(DisplayName = "Legs"),
    Feet  UMETA(DisplayName = "Feet"),
    Hair  UMETA(DisplayName = "Hair"),   // still keep for materials
    Eyes  UMETA(DisplayName = "Eyes"),
    Mouth UMETA(DisplayName = "Mouth")
};

// Hair attachment types
UENUM(BlueprintType)
enum class EHairAttachmentType : uint8
{
    None         UMETA(DisplayName = "None"),
    StaticMesh   UMETA(DisplayName = "Static Mesh Hair"),
    SkeletalMesh UMETA(DisplayName = "Skeletal Mesh Hair"),
    Groom        UMETA(DisplayName = "Groom Hair"),
    Hybrid       UMETA(DisplayName = "Hybrid (Mesh + Groom)")
};


// Add this to your header file if using C++ enum
UENUM(BlueprintType)
enum class ECharacterSelectionState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    SteppingForward UMETA(DisplayName = "Stepping Forward"), 
    SteppingBackward UMETA(DisplayName = "Stepping Backward")
};


//
// === HAIR ATTACHMENT DATA ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FHairAttachmentData
{
    GENERATED_BODY();

    // Attachment method
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment")
    EHairAttachmentType AttachmentType = EHairAttachmentType::Groom;

    // Groom assets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Groom", meta = (EditCondition = "AttachmentType == EHairAttachmentType::Groom || AttachmentType == EHairAttachmentType::Hybrid"))
    TSoftObjectPtr<UGroomAsset> GroomAsset = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Groom", meta = (EditCondition = "AttachmentType == EHairAttachmentType::Groom || AttachmentType == EHairAttachmentType::Hybrid"))
    TSoftObjectPtr<UGroomBindingAsset> GroomBindingAsset = nullptr;

    // Mesh assets (for fallback or hybrid)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Mesh", meta = (EditCondition = "AttachmentType == EHairAttachmentType::StaticMesh || AttachmentType == EHairAttachmentType::SkeletalMesh || AttachmentType == EHairAttachmentType::Hybrid"))
    TSoftObjectPtr<USkeletalMesh> HairMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Mesh", meta = (EditCondition = "AttachmentType == EHairAttachmentType::StaticMesh || AttachmentType == EHairAttachmentType::SkeletalMesh || AttachmentType == EHairAttachmentType::Hybrid"))
    TSoftObjectPtr<UMaterialInterface> HairMaterial = nullptr;

    // Attachment settings (bone name, not socket)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment")
    FName AttachmentBoneName = TEXT("head");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment")
    FTransform RelativeTransform = FTransform::Identity;

    // Groom specific settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Groom Settings", meta = (EditCondition = "AttachmentType == EHairAttachmentType::Groom || AttachmentType == EHairAttachmentType::Hybrid"))
    bool bEnableSimulation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Groom Settings", meta = (EditCondition = "AttachmentType == EHairAttachmentType::Groom || AttachmentType == EHairAttachmentType::Hybrid"))
    bool bEnableGlobalInterpolation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Groom Settings", meta = (EditCondition = "AttachmentType == EHairAttachmentType::Groom || AttachmentType == EHairAttachmentType::Hybrid"))
    float GroomLODBias = 0.0f;

    // Rendering settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Rendering")
    bool bCastShadow = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Rendering")
    bool bReceivesDecals = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Rendering")
    float CullDistance = 0.0f;

    FHairAttachmentData()
    {
        AttachmentType = EHairAttachmentType::Groom;
        AttachmentBoneName = TEXT("head");
        RelativeTransform = FTransform::Identity;
        bEnableSimulation = true;
        bEnableGlobalInterpolation = true;
        GroomLODBias = 0.0f;
        bCastShadow = true;
        bReceivesDecals = false;
        CullDistance = 0.0f;
    }
};

//
// === BASE PRESET ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText DisplayName = FText::FromString(TEXT("Unnamed Preset"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    TObjectPtr<UTexture2D> PreviewThumbnail = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 UnlockLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    bool bRequiresPremium = false;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FRacePreset : public FBasePreset
{
    GENERATED_BODY();

    // Core race identity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
    ECharacterRace RaceType = ECharacterRace::Hume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
    ECharacterGender DefaultGender = ECharacterGender::Male;

    // NEW: Default height morph value for this race
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Physical Traits", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DefaultHeight = 0.0f;

    // Modular meshes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Assets")
    TSoftObjectPtr<USkeletalMesh> HeadMesh = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Assets")
    TSoftObjectPtr<USkeletalMesh> BodyMesh = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Assets")
    TSoftObjectPtr<USkeletalMesh> HandsMesh = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Assets")
    TSoftObjectPtr<USkeletalMesh> LegsMesh = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Assets")
    TSoftObjectPtr<USkeletalMesh> FeetMesh = nullptr;

    // Race defining hair - one groom per race for consistent appearance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Hair")
    TSoftObjectPtr<UGroomAsset> RaceHairGroom = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Hair")
    TSoftObjectPtr<UGroomBindingAsset> RaceHairBinding = nullptr;

    // UI Presentation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race UI")
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race UI")
    TObjectPtr<UTexture2D> Portrait = nullptr;

    // Animations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Animations")
    TSoftObjectPtr<UAnimationAsset> IdleAnimation = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Animations")
    TSoftObjectPtr<UAnimMontage> SelectAnimation = nullptr;

    // Simple check if race has hair
    bool HasRaceHair() const
    {
        return RaceHairGroom != nullptr;
    }
};


//
// === FEATURE PRESETS HAIR ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FHairPreset : public FBasePreset
{
    GENERATED_BODY();

    // Hair attachment data - supports all hair types
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment")
    FHairAttachmentData HairAttachment;

    // Gender-specific hair attachments (optional - for when hair differs by gender)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment - Gender Specific")
    bool bUseGenderSpecificHair = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment - Gender Specific", meta = (EditCondition = "bUseGenderSpecificHair"))
    FHairAttachmentData MaleHairAttachment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Attachment - Gender Specific", meta = (EditCondition = "bUseGenderSpecificHair"))
    FHairAttachmentData FemaleHairAttachment;

    // Hair properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Properties")
    float HairLength = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Properties")
    float HairVolume = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Properties")
    bool bSupportsColorCustomization = true;

    // Hair physics and behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Physics")
    float HairStiffness = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Physics")
    float HairDamping = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Physics")
    bool bEnableHairCollision = true;

    // Helper function to get the appropriate hair attachment for a given gender
    FHairAttachmentData GetHairAttachmentForGender(ECharacterGender Gender) const
    {
        if (bUseGenderSpecificHair)
        {
            return (Gender == ECharacterGender::Male) ? MaleHairAttachment : FemaleHairAttachment;
        }
        return HairAttachment;
    }
};


USTRUCT(BlueprintType)
struct DAWNBLADE_API FEyePreset : public FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye Properties")
    float EyeSize = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye Properties")
    float EyeSpacing = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye Properties")
    float EyeHeight = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye Properties")
    float EyeDepth = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eye Assets")
    TSoftObjectPtr<UMaterialInterface> EyeMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FNosePreset : public FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nose Properties")
    float NoseWidth = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nose Properties")
    float NoseLength = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nose Properties")
    float NoseBridge = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nose Properties")
    float NostrilSize = 0.5f;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FMouthPreset : public FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouth Properties")
    float MouthWidth = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouth Properties")
    float LipThickness = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouth Properties")
    float LipCurvature = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouth Assets")
    TSoftObjectPtr<UMaterialInterface> LipMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FBodyPreset : public FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Properties")
    float Height = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Properties")
    float Weight = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Properties")
    float Muscle = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Properties")
    float ChestSize = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Properties")
    float WaistSize = 0.5f;

    // Modular meshes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Assets")
    TSoftObjectPtr<USkeletalMesh> HeadMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Assets")
    TSoftObjectPtr<USkeletalMesh> BodyMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Assets")
    TSoftObjectPtr<USkeletalMesh> HandsMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Assets")
    TSoftObjectPtr<USkeletalMesh> LegsMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Assets")
    TSoftObjectPtr<USkeletalMesh> FeetMesh = nullptr;

    // Body-specific hair attachment (like chest hair, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Hair")
    FHairAttachmentData BodyHairAttachment;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FMakeupPreset : public FBasePreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Makeup Properties")
    float MakeupIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Makeup Assets")
    TSoftObjectPtr<UMaterialInterface> MakeupMaterial = nullptr;
};

//
// === COLOR PRESET ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FCharacterColorPreset
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    FLinearColor Color = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText ColorName = FText::FromString(TEXT("Unnamed Color"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 UnlockLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    bool bRequiresPremium = false;

    FCharacterColorPreset() {}
    FCharacterColorPreset(FLinearColor InColor, const FString& InName, int32 InUnlockLevel = 1, bool bInRequiresPremium = false)
        : Color(InColor), ColorName(FText::FromString(InName)), UnlockLevel(InUnlockLevel), bRequiresPremium(bInRequiresPremium) {}
};

//
// === APPEARANCE DATA ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FCharacterMeshSlots
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Slots")
    int32 HeadPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Slots")
    int32 BodyPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Slots")
    int32 HandsPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Slots")
    int32 LegsPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Slots")
    int32 FeetPresetIndex = 0;
};

USTRUCT(BlueprintType)
struct DAWNBLADE_API FCharacterAppearanceData
{
    GENERATED_BODY();

    // Metadata
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterName = TEXT("New Character");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterGender Gender = ECharacterGender::Male;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterRace RaceId = ECharacterRace::Hume;

    // Presets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    int32 HairPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    int32 EyePresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    int32 NosePresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    int32 MouthPresetIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    int32 MakeupPresetIndex = 0;

    // Modular body slot indices
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
    FCharacterMeshSlots MeshSlots;

    // Colors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor HairColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor EyeColor = FLinearColor::Blue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor SkinTone = FLinearColor(0.8f, 0.6f, 0.4f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor LipColor = FLinearColor(0.9f, 0.4f, 0.4f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor MakeupColor = FLinearColor::White;

    // Fine tuning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fine Tuning")
    float HairShade = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fine Tuning")
    float HairTransparency = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fine Tuning")
    float HairTextureDetail = 0.5f;

    // Hair attachment override (for custom hair beyond presets)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Override")
    bool bUseCustomHairAttachment = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hair Override", meta = (EditCondition = "bUseCustomHairAttachment"))
    FHairAttachmentData CustomHairAttachment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    FString CustomizationVersion = TEXT("1.0.0");
};

//
// === COMPRESSED APPEARANCE (for MMO net traffic) ===
//
USTRUCT(BlueprintType)
struct DAWNBLADE_API FCompressedAppearanceData
{
    GENERATED_BODY();

    UPROPERTY() uint64 PackedPresetIndices = 0;
    UPROPERTY() uint64 PackedHairColor = 0;
    UPROPERTY() uint64 PackedEyeColor = 0;
    UPROPERTY() uint64 PackedSkinTone = 0;
    UPROPERTY() uint64 PackedLipColor = 0;
    UPROPERTY() uint32 PackedProperties = 0;
    UPROPERTY() uint8 Gender = 0;
    UPROPERTY() uint8 RaceId = static_cast<uint8>(ECharacterRace::Hume);
    
    // Hair attachment type for compression
    UPROPERTY() uint8 HairAttachmentType = static_cast<uint8>(EHairAttachmentType::None);
    UPROPERTY() uint8 bUseCustomHairAttachment = 0;
};

//
// === HAIR ATTACHMENT UTILITY FUNCTIONS ===
//
UCLASS(BlueprintType, Blueprintable)
class DAWNBLADE_API UHairAttachmentUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Blueprint-callable utility functions for creating hair attachments
    UFUNCTION(BlueprintCallable, Category = "Hair Attachment")
    static bool IsValidHairAttachment(const FHairAttachmentData& HairAttachment);

    UFUNCTION(BlueprintCallable, Category = "Hair Attachment") 
    static FHairAttachmentData CreateGroomAttachment(UGroomAsset* GroomAsset, UGroomBindingAsset* BindingAsset = nullptr, const FName& BoneName = TEXT("head"));

    UFUNCTION(BlueprintCallable, Category = "Hair Attachment")
    static FHairAttachmentData CreateMeshAttachment(USkeletalMesh* HairMesh, UMaterialInterface* HairMaterial = nullptr, const FName& BoneName = TEXT("head"));

    UFUNCTION(BlueprintCallable, Category = "Hair Attachment")
    static FHairAttachmentData CreateHybridAttachment(UGroomAsset* GroomAsset, USkeletalMesh* HairMesh, UGroomBindingAsset* BindingAsset = nullptr, UMaterialInterface* HairMaterial = nullptr, const FName& BoneName = TEXT("head"));

    // Blueprint wrappers for struct functions
    UFUNCTION(BlueprintCallable, Category = "Hair Preset", CallInEditor)
    static FHairAttachmentData GetHairPresetAttachmentForGender(const FHairPreset& HairPreset, ECharacterGender Gender);
};

// === Conversion ===
FCompressedAppearanceData CompressAppearanceData(const FCharacterAppearanceData& AppearanceData);
FCharacterAppearanceData DecompressAppearanceData(const FCompressedAppearanceData& Compressed);