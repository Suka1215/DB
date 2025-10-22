// CharacterCreatorComponent.cpp

#include "CharacterCreatorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "JsonObjectConverter.h"

UCharacterCreatorComponent::UCharacterCreatorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    CurrentAppearance = FCharacterAppearanceData();
}

void UCharacterCreatorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        BaseCharacterMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
    }

    UpdateCharacterMeshes();
    bInitialized = true;
}

void UCharacterCreatorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCharacterCreatorComponent, CurrentAppearance);
}

// === Public API ===

void UCharacterCreatorComponent::ApplyAppearance(const FCharacterAppearanceData& InAppearanceData)
{
    if (GetOwner()->HasAuthority())
    {
        FString ErrorMessage;
        if (ValidateAppearanceData(InAppearanceData, ErrorMessage))
        {
            CurrentAppearance = InAppearanceData;
            MulticastApplyAppearance(CurrentAppearance);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid appearance data: %s"), *ErrorMessage);
        }
    }
    else
    {
        ServerApplyAppearance(InAppearanceData);
    }
}

void UCharacterCreatorComponent::ApplySlotMesh(ECharacterMeshSlot Slot, int32 PresetIndex)
{
    if (!CreatorDataAsset) return;

    if (!CreatorDataAsset->GetBodyPresets().IsValidIndex(PresetIndex)) return;

    const FBodyPreset& BodyPreset = CreatorDataAsset->GetBodyPresets()[PresetIndex];
    TSoftObjectPtr<USkeletalMesh> SelectedMesh = nullptr;

    switch (Slot)
    {
        case ECharacterMeshSlot::Head:  SelectedMesh = BodyPreset.HeadMesh; break;
        case ECharacterMeshSlot::Body:  SelectedMesh = BodyPreset.BodyMesh; break;
        case ECharacterMeshSlot::Hands: SelectedMesh = BodyPreset.HandsMesh; break;
        case ECharacterMeshSlot::Legs:  SelectedMesh = BodyPreset.LegsMesh; break;
        case ECharacterMeshSlot::Feet:  SelectedMesh = BodyPreset.FeetMesh; break;
        default: break;
    }

    if (SelectedMesh.IsValid())
    {
        USkeletalMeshComponent** TargetComp = nullptr;

        switch (Slot)
        {
            case ECharacterMeshSlot::Head:  TargetComp = &HeadMeshComponent; break;
            case ECharacterMeshSlot::Body:  TargetComp = &BodyMeshComponent; break;
            case ECharacterMeshSlot::Hands: TargetComp = &HandsMeshComponent; break;
            case ECharacterMeshSlot::Legs:  TargetComp = &LegsMeshComponent; break;
            case ECharacterMeshSlot::Feet:  TargetComp = &FeetMeshComponent; break;
            default: break;
        }

        if (TargetComp)
        {
            if (!*TargetComp && GetOwner())
            {
                *TargetComp = NewObject<USkeletalMeshComponent>(GetOwner(), NAME_None);
                (*TargetComp)->RegisterComponent();
                (*TargetComp)->AttachToComponent(BaseCharacterMesh, FAttachmentTransformRules::KeepRelativeTransform);
                (*TargetComp)->SetLeaderPoseComponent(BaseCharacterMesh);
            }

            if (*TargetComp)
            {
                (*TargetComp)->SetSkeletalMesh(SelectedMesh.LoadSynchronous());
            }
        }
    }

    UpdateMaterialParameters();
    BroadcastAppearanceChanged();
}

void UCharacterCreatorComponent::SetSlotColor(ECharacterMeshSlot Slot, FLinearColor Color)
{
    switch (Slot)
    {
        case ECharacterMeshSlot::Hair:   CurrentAppearance.HairColor = Color; break;
        case ECharacterMeshSlot::Eyes:   CurrentAppearance.EyeColor = Color; break;
        case ECharacterMeshSlot::Body:   CurrentAppearance.SkinTone = Color; break;
        case ECharacterMeshSlot::Mouth:  CurrentAppearance.LipColor = Color; break;
        case ECharacterMeshSlot::Head:   CurrentAppearance.MakeupColor = Color; break;
        default: break;
    }

    UpdateMaterialParameters();
    BroadcastAppearanceChanged();
}

void UCharacterCreatorComponent::RandomizeCharacter()
{
    if (!CreatorDataAsset) return;

    FCharacterAppearanceData RandomData = CurrentAppearance;

    // Randomize one body preset index for all slots
    const TArray<FBodyPreset>& BodyPresets = CreatorDataAsset->GetBodyPresets();
    if (!BodyPresets.IsEmpty())
    {
        int32 RandIndex = FMath::RandRange(0, BodyPresets.Num() - 1);
        RandomData.MeshSlots.HeadPresetIndex  = RandIndex;
        RandomData.MeshSlots.BodyPresetIndex  = RandIndex;
        RandomData.MeshSlots.HandsPresetIndex = RandIndex;
        RandomData.MeshSlots.LegsPresetIndex  = RandIndex;
        RandomData.MeshSlots.FeetPresetIndex  = RandIndex;
    }

    // Randomize colors
    auto RandomColor = [](const TArray<FCharacterColorPreset>& Colors) -> FLinearColor
    {
        return Colors.IsEmpty() ? FLinearColor::White : Colors[FMath::RandRange(0, Colors.Num() - 1)].Color;
    };

    RandomData.HairColor   = RandomColor(CreatorDataAsset->GetColorsForCategory(ECharacterFeatureCategory::Hair));
    RandomData.EyeColor    = RandomColor(CreatorDataAsset->GetColorsForCategory(ECharacterFeatureCategory::Eyes));
    RandomData.SkinTone    = RandomColor(CreatorDataAsset->GetColorsForCategory(ECharacterFeatureCategory::Body));
    RandomData.LipColor    = RandomColor(CreatorDataAsset->GetColorsForCategory(ECharacterFeatureCategory::Mouth));
    RandomData.MakeupColor = RandomColor(CreatorDataAsset->GetColorsForCategory(ECharacterFeatureCategory::Makeup));

    ApplyAppearance(RandomData);
}

void UCharacterCreatorComponent::ResetToDefault()
{
    FCharacterAppearanceData DefaultData;
    DefaultData.CharacterName = CurrentAppearance.CharacterName;
    DefaultData.Gender = CurrentAppearance.Gender;
    DefaultData.RaceId = CurrentAppearance.RaceId;
    ApplyAppearance(DefaultData);
}

// === Network ===

bool UCharacterCreatorComponent::ServerApplyAppearance_Validate(const FCharacterAppearanceData& InAppearanceData)
{
    FString ErrorMessage;
    return ValidateAppearanceData(InAppearanceData, ErrorMessage);
}

void UCharacterCreatorComponent::ServerApplyAppearance_Implementation(const FCharacterAppearanceData& InAppearanceData)
{
    ApplyAppearance(InAppearanceData);
}

void UCharacterCreatorComponent::MulticastApplyAppearance_Implementation(const FCharacterAppearanceData& InAppearanceData)
{
    CurrentAppearance = InAppearanceData;
    UpdateCharacterMeshes();
    BroadcastAppearanceChanged();
}

// === Persistence ===

void UCharacterCreatorComponent::SaveCharacterToDatabase(int64 PlayerId)
{
    FString JsonString;
    FJsonObjectConverter::UStructToJsonObjectString(CurrentAppearance, JsonString);
    UE_LOG(LogTemp, Log, TEXT("Saving character for player %lld: %s"), PlayerId, *JsonString);
}

void UCharacterCreatorComponent::LoadCharacterFromDatabase(int64 PlayerId)
{
    UE_LOG(LogTemp, Log, TEXT("Loading character from DB for player %lld"), PlayerId);
}

// === Validation ===

bool UCharacterCreatorComponent::ValidateAppearanceData(const FCharacterAppearanceData& Data, FString& OutErrorMessage) const
{
    if (!CreatorDataAsset) return true;

    const TArray<FBodyPreset>& Presets = CreatorDataAsset->GetBodyPresets();

    if (!Presets.IsValidIndex(Data.MeshSlots.HeadPresetIndex))  { OutErrorMessage = TEXT("Invalid head preset index");  return false; }
    if (!Presets.IsValidIndex(Data.MeshSlots.BodyPresetIndex))  { OutErrorMessage = TEXT("Invalid body preset index");  return false; }
    if (!Presets.IsValidIndex(Data.MeshSlots.HandsPresetIndex)) { OutErrorMessage = TEXT("Invalid hands preset index"); return false; }
    if (!Presets.IsValidIndex(Data.MeshSlots.LegsPresetIndex))  { OutErrorMessage = TEXT("Invalid legs preset index");  return false; }
    if (!Presets.IsValidIndex(Data.MeshSlots.FeetPresetIndex))  { OutErrorMessage = TEXT("Invalid feet preset index");  return false; }

    return true;
}

// === Internal ===

void UCharacterCreatorComponent::OnRep_CharacterAppearance()
{
    UpdateCharacterMeshes();
    BroadcastAppearanceChanged();
}

void UCharacterCreatorComponent::BroadcastAppearanceChanged()
{
    OnCharacterAppearanceChanged.Broadcast(CurrentAppearance);
}

void UCharacterCreatorComponent::UpdateCharacterMeshes()
{
    if (!BaseCharacterMesh || !bInitialized || !CreatorDataAsset) return;

    CreateDynamicMaterials();
    ApplyPresetMorphTargets();
    UpdateMaterialParameters();

    ApplySlotMesh(ECharacterMeshSlot::Head,  CurrentAppearance.MeshSlots.HeadPresetIndex);
    ApplySlotMesh(ECharacterMeshSlot::Body,  CurrentAppearance.MeshSlots.BodyPresetIndex);
    ApplySlotMesh(ECharacterMeshSlot::Hands, CurrentAppearance.MeshSlots.HandsPresetIndex);
    ApplySlotMesh(ECharacterMeshSlot::Legs,  CurrentAppearance.MeshSlots.LegsPresetIndex);
    ApplySlotMesh(ECharacterMeshSlot::Feet,  CurrentAppearance.MeshSlots.FeetPresetIndex);
}

void UCharacterCreatorComponent::ApplyPresetMorphTargets()
{
    if (!BaseCharacterMesh || !BaseCharacterMesh->GetSkeletalMeshAsset() || !CreatorDataAsset) return;

    if (CreatorDataAsset->GetBodyPresets().IsValidIndex(CurrentAppearance.MeshSlots.BodyPresetIndex))
    {
        const FBodyPreset& BodyPreset = CreatorDataAsset->GetBodyPresets()[CurrentAppearance.MeshSlots.BodyPresetIndex];
        BaseCharacterMesh->SetMorphTarget(TEXT("Height"), BodyPreset.Height);
        BaseCharacterMesh->SetMorphTarget(TEXT("Weight"), BodyPreset.Weight);
        BaseCharacterMesh->SetMorphTarget(TEXT("Muscle"), BodyPreset.Muscle);
        BaseCharacterMesh->SetMorphTarget(TEXT("ChestSize"), BodyPreset.ChestSize);
        BaseCharacterMesh->SetMorphTarget(TEXT("WaistSize"), BodyPreset.WaistSize);
    }
}

void UCharacterCreatorComponent::UpdateMaterialParameters()
{
    if (!BaseCharacterMesh) return;

    if (UMaterialInstanceDynamic* HeadMat = DynamicMaterials.FindRef(HeadMaterialSlot))
    {
        HeadMat->SetVectorParameterValue(TEXT("SkinTone"), CurrentAppearance.SkinTone);
        HeadMat->SetVectorParameterValue(TEXT("LipColor"), CurrentAppearance.LipColor);
        HeadMat->SetVectorParameterValue(TEXT("MakeupColor"), CurrentAppearance.MakeupColor);
    }

    if (UMaterialInstanceDynamic* BodyMat = DynamicMaterials.FindRef(BodyMaterialSlot))
    {
        BodyMat->SetVectorParameterValue(TEXT("SkinTone"), CurrentAppearance.SkinTone);
    }

    if (UMaterialInstanceDynamic* EyeMat = DynamicMaterials.FindRef(EyeMaterialSlot))
    {
        EyeMat->SetVectorParameterValue(TEXT("IrisColor"), CurrentAppearance.EyeColor);
    }

    if (HairMaterialSlot >= 0 && HairMaterialSlot < BaseCharacterMesh->GetNumMaterials())
    {
        if (UMaterialInstanceDynamic* HairMat = BaseCharacterMesh->CreateAndSetMaterialInstanceDynamic(HairMaterialSlot))
        {
            HairMat->SetVectorParameterValue(TEXT("HairColor"), CurrentAppearance.HairColor);
            HairMat->SetScalarParameterValue(TEXT("Shade"), CurrentAppearance.HairShade);
            HairMat->SetScalarParameterValue(TEXT("Transparency"), CurrentAppearance.HairTransparency);
            HairMat->SetScalarParameterValue(TEXT("TextureDetail"), CurrentAppearance.HairTextureDetail);
        }
    }
}

void UCharacterCreatorComponent::CreateDynamicMaterials()
{
    if (!BaseCharacterMesh) return;

    TArray<int32> MaterialSlots = { HeadMaterialSlot, BodyMaterialSlot, EyeMaterialSlot };
    for (int32 SlotIndex : MaterialSlots)
    {
        if (!DynamicMaterials.Contains(SlotIndex))
        {
            UMaterialInstanceDynamic* DynMat = BaseCharacterMesh->CreateAndSetMaterialInstanceDynamic(SlotIndex);
            if (DynMat)
            {
                DynamicMaterials.Add(SlotIndex, DynMat);
            }
        }
    }
}
