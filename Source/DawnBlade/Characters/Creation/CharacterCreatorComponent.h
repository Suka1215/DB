// CharacterCreatorComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCreatorDataAsset.h"
#include "Types/CharacterCreatorTypes.h"
#include "CharacterCreatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterAppearanceChanged, const FCharacterAppearanceData&, AppearanceData);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DAWNBLADE_API UCharacterCreatorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterCreatorComponent();

protected:
    virtual void BeginPlay() override;

public:
    // === Events ===
    UPROPERTY(BlueprintAssignable, Category = "Character Creator")
    FOnCharacterAppearanceChanged OnCharacterAppearanceChanged;

    // === Current Character Data ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Creator", ReplicatedUsing = OnRep_CharacterAppearance)
    FCharacterAppearanceData CurrentAppearance;

    // === Configuration ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Creator")
    UCharacterCreatorDataAsset* CreatorDataAsset;

    // === Material Slots ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
    int32 HeadMaterialSlot = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
    int32 BodyMaterialSlot = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
    int32 HairMaterialSlot = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
    int32 EyeMaterialSlot = 3;

    // === Main Functions ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void ApplyAppearance(const FCharacterAppearanceData& InAppearanceData);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void RandomizeCharacter();

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void ResetToDefault();

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FCharacterAppearanceData GetCurrentAppearance() const { return CurrentAppearance; }

    // === Slot Functions ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void ApplySlotMesh(ECharacterMeshSlot Slot, int32 PresetIndex);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetSlotColor(ECharacterMeshSlot Slot, FLinearColor Color);

    // === Network ===
    UFUNCTION(Server, Reliable, WithValidation, Category = "Character Creator")
    void ServerApplyAppearance(const FCharacterAppearanceData& InAppearanceData);

    UFUNCTION(NetMulticast, Reliable, Category = "Character Creator")
    void MulticastApplyAppearance(const FCharacterAppearanceData& InAppearanceData);

    // === Persistence ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SaveCharacterToDatabase(int64 PlayerId);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void LoadCharacterFromDatabase(int64 PlayerId);

    // === Validation ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    bool ValidateAppearanceData(const FCharacterAppearanceData& Data, FString& OutErrorMessage) const;

protected:
    // Replication
    UFUNCTION()
    void OnRep_CharacterAppearance();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Internal
    void BroadcastAppearanceChanged();
    void UpdateCharacterMeshes();
    void ApplyPresetMorphTargets();
    void UpdateMaterialParameters();
    void CreateDynamicMaterials();

private:
    // Root skeletal mesh (base body)
    UPROPERTY()
    USkeletalMeshComponent* BaseCharacterMesh;

    // Per-slot mesh components
    UPROPERTY()
    USkeletalMeshComponent* HeadMeshComponent;

    UPROPERTY()
    USkeletalMeshComponent* BodyMeshComponent;

    UPROPERTY()
    USkeletalMeshComponent* HandsMeshComponent;

    UPROPERTY()
    USkeletalMeshComponent* LegsMeshComponent;

    UPROPERTY()
    USkeletalMeshComponent* FeetMeshComponent;

    // Dynamic materials
    UPROPERTY()
    TMap<int32, UMaterialInstanceDynamic*> DynamicMaterials;

    bool bInitialized = false;
};
