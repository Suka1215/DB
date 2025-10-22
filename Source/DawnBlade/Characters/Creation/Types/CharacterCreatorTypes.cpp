// CharacterCreatorTypes.cpp

#include "Characters/Creation/Types/CharacterCreatorTypes.h"

//
// === CONSTANTS ===
//
namespace
{
    constexpr int32 BitsPerPresetIndex = 6;                // allows up to 64 presets per slot
    constexpr uint64 PresetIndexMask   = (1 << BitsPerPresetIndex) - 1; // 0x3F
}

//
// === HAIR ATTACHMENT UTILITY FUNCTIONS ===
//
bool UHairAttachmentUtils::IsValidHairAttachment(const FHairAttachmentData& HairAttachment)
{
    switch (HairAttachment.AttachmentType)
    {
        case EHairAttachmentType::None:
            return true; // No attachment is valid
            
        case EHairAttachmentType::Groom:
            return HairAttachment.GroomAsset != nullptr;
            
        case EHairAttachmentType::StaticMesh:
        case EHairAttachmentType::SkeletalMesh:
            return HairAttachment.HairMesh != nullptr;
            
        case EHairAttachmentType::Hybrid:
            // For hybrid, we need at least one of the components
            return (HairAttachment.GroomAsset != nullptr) || (HairAttachment.HairMesh != nullptr);
            
        default:
            return false;
    }
}

FHairAttachmentData UHairAttachmentUtils::CreateGroomAttachment(UGroomAsset* GroomAsset, UGroomBindingAsset* BindingAsset, const FName& BoneName)
{
    FHairAttachmentData Attachment;
    Attachment.AttachmentType = EHairAttachmentType::Groom;
    Attachment.GroomAsset = GroomAsset;
    Attachment.GroomBindingAsset = BindingAsset;
    Attachment.AttachmentBoneName = BoneName;
    Attachment.RelativeTransform = FTransform::Identity;
    Attachment.bEnableSimulation = true;
    Attachment.bEnableGlobalInterpolation = true;
    Attachment.GroomLODBias = 0.0f;
    Attachment.bCastShadow = true;
    Attachment.bReceivesDecals = false;
    Attachment.CullDistance = 0.0f;
    
    return Attachment;
}

FHairAttachmentData UHairAttachmentUtils::CreateMeshAttachment(USkeletalMesh* HairMesh, UMaterialInterface* HairMaterial, const FName& BoneName)
{
    FHairAttachmentData Attachment;
    Attachment.AttachmentType = EHairAttachmentType::SkeletalMesh;
    Attachment.HairMesh = HairMesh;
    Attachment.HairMaterial = HairMaterial;
    Attachment.AttachmentBoneName = BoneName;
    Attachment.RelativeTransform = FTransform::Identity;
    Attachment.bCastShadow = true;
    Attachment.bReceivesDecals = false;
    Attachment.CullDistance = 0.0f;
    
    return Attachment;
}

FHairAttachmentData UHairAttachmentUtils::CreateHybridAttachment(UGroomAsset* GroomAsset, USkeletalMesh* HairMesh, UGroomBindingAsset* BindingAsset, UMaterialInterface* HairMaterial, const FName& BoneName)
{
    FHairAttachmentData Attachment;
    Attachment.AttachmentType = EHairAttachmentType::Hybrid;
    Attachment.GroomAsset = GroomAsset;
    Attachment.GroomBindingAsset = BindingAsset;
    Attachment.HairMesh = HairMesh;
    Attachment.HairMaterial = HairMaterial;
    Attachment.AttachmentBoneName = BoneName;
    Attachment.RelativeTransform = FTransform::Identity;
    Attachment.bEnableSimulation = true;
    Attachment.bEnableGlobalInterpolation = true;
    Attachment.GroomLODBias = 0.0f;
    Attachment.bCastShadow = true;
    Attachment.bReceivesDecals = false;
    Attachment.CullDistance = 0.0f;
    
    return Attachment;
}

// Blueprint wrapper functions for struct methods
FHairAttachmentData UHairAttachmentUtils::GetHairPresetAttachmentForGender(const FHairPreset& HairPreset, ECharacterGender Gender)
{
    return HairPreset.GetHairAttachmentForGender(Gender);
}

//
// === COMPRESSION ===
//
FCompressedAppearanceData CompressAppearanceData(const FCharacterAppearanceData& AppearanceData)
{
    FCompressedAppearanceData Compressed;

    // Pack preset indices into one 64-bit value
    Compressed.PackedPresetIndices =
        ((uint64)FMath::Clamp(AppearanceData.HairPresetIndex, 0, (int32)PresetIndexMask) << (BitsPerPresetIndex * 0)) |
        ((uint64)FMath::Clamp(AppearanceData.EyePresetIndex, 0, (int32)PresetIndexMask)  << (BitsPerPresetIndex * 1)) |
        ((uint64)FMath::Clamp(AppearanceData.NosePresetIndex, 0, (int32)PresetIndexMask) << (BitsPerPresetIndex * 2)) |
        ((uint64)FMath::Clamp(AppearanceData.MouthPresetIndex, 0, (int32)PresetIndexMask)<< (BitsPerPresetIndex * 3)) |
        ((uint64)FMath::Clamp(AppearanceData.MeshSlots.BodyPresetIndex, 0, (int32)PresetIndexMask) << (BitsPerPresetIndex * 4)) |
        ((uint64)FMath::Clamp(AppearanceData.MakeupPresetIndex, 0, (int32)PresetIndexMask)<< (BitsPerPresetIndex * 5));

    // Pack FLinearColor -> 64 bits (RGBA16)
    auto PackColor = [](const FLinearColor& Color) -> uint64
    {
        uint16 R = (uint16)(FMath::Clamp(Color.R, 0.f, 1.f) * 65535);
        uint16 G = (uint16)(FMath::Clamp(Color.G, 0.f, 1.f) * 65535);
        uint16 B = (uint16)(FMath::Clamp(Color.B, 0.f, 1.f) * 65535);
        uint16 A = (uint16)(FMath::Clamp(Color.A, 0.f, 1.f) * 65535);
        return ((uint64)R << 0) | ((uint64)G << 16) | ((uint64)B << 32) | ((uint64)A << 48);
    };

    Compressed.PackedHairColor = PackColor(AppearanceData.HairColor);
    Compressed.PackedEyeColor  = PackColor(AppearanceData.EyeColor);
    Compressed.PackedSkinTone  = PackColor(AppearanceData.SkinTone);
    Compressed.PackedLipColor  = PackColor(AppearanceData.LipColor);

    // Pack hair fine-tuning into one 32-bit int
    uint8 Shade        = (uint8)(FMath::Clamp(AppearanceData.HairShade, 0.f, 1.f) * 255);
    uint8 Transparency = (uint8)(FMath::Clamp(AppearanceData.HairTransparency, 0.f, 1.f) * 255);
    uint8 Detail       = (uint8)(FMath::Clamp(AppearanceData.HairTextureDetail, 0.f, 1.f) * 255);

    Compressed.PackedProperties =
        ((uint32)Shade        << 0)  |
        ((uint32)Transparency << 8)  |
        ((uint32)Detail       << 16);

    // Gender & Race
    Compressed.Gender = static_cast<uint8>(AppearanceData.Gender);
    Compressed.RaceId = static_cast<uint8>(AppearanceData.RaceId);

    // Compress hair attachment data
    if (AppearanceData.bUseCustomHairAttachment)
    {
        Compressed.HairAttachmentType = static_cast<uint8>(AppearanceData.CustomHairAttachment.AttachmentType);
        Compressed.bUseCustomHairAttachment = 1;
    }
    else
    {
        Compressed.HairAttachmentType = static_cast<uint8>(EHairAttachmentType::None);
        Compressed.bUseCustomHairAttachment = 0;
    }

    return Compressed;
}

//
// === DECOMPRESSION ===
//
FCharacterAppearanceData DecompressAppearanceData(const FCompressedAppearanceData& Compressed)
{
    FCharacterAppearanceData Data;

    // Unpack preset indices
    Data.HairPresetIndex          = (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 0)) & PresetIndexMask);
    Data.EyePresetIndex           = (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 1)) & PresetIndexMask);
    Data.NosePresetIndex          = (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 2)) & PresetIndexMask);
    Data.MouthPresetIndex         = (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 3)) & PresetIndexMask);
    Data.MeshSlots.BodyPresetIndex= (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 4)) & PresetIndexMask);
    Data.MakeupPresetIndex        = (int32)((Compressed.PackedPresetIndices >> (BitsPerPresetIndex * 5)) & PresetIndexMask);

    // Unpack 64-bit color back into FLinearColor
    auto UnpackColor = [](uint64 Packed) -> FLinearColor
    {
        uint16 R = (Packed >> 0)  & 0xFFFF;
        uint16 G = (Packed >> 16) & 0xFFFF;
        uint16 B = (Packed >> 32) & 0xFFFF;
        uint16 A = (Packed >> 48) & 0xFFFF;
        return FLinearColor(
            (float)R / 65535.f,
            (float)G / 65535.f,
            (float)B / 65535.f,
            (float)A / 65535.f
        );
    };

    Data.HairColor = UnpackColor(Compressed.PackedHairColor);
    Data.EyeColor  = UnpackColor(Compressed.PackedEyeColor);
    Data.SkinTone  = UnpackColor(Compressed.PackedSkinTone);
    Data.LipColor  = UnpackColor(Compressed.PackedLipColor);

    // Unpack fine-tuning
    Data.HairShade         = ((Compressed.PackedProperties >> 0)  & 0xFF) / 255.f;
    Data.HairTransparency  = ((Compressed.PackedProperties >> 8)  & 0xFF) / 255.f;
    Data.HairTextureDetail = ((Compressed.PackedProperties >> 16) & 0xFF) / 255.f;

    // Gender & Race
    Data.Gender = static_cast<ECharacterGender>(Compressed.Gender);
    Data.RaceId = static_cast<ECharacterRace>(Compressed.RaceId);

    // Decompress hair attachment data
    Data.bUseCustomHairAttachment = (Compressed.bUseCustomHairAttachment != 0);
    if (Data.bUseCustomHairAttachment)
    {
        Data.CustomHairAttachment.AttachmentType = static_cast<EHairAttachmentType>(Compressed.HairAttachmentType);
        // Note: Full hair attachment data (assets, etc.) cannot be compressed efficiently
        // This only stores the attachment type - full data would need to be looked up from presets/database
    }

    return Data;
}