// DBEquipmentComponent.h - UPDATED FOR DATA-DRIVEN WEAPON ATTACHMENT
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/DBEquipmentItemData.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h" 
#include "GameplayTagContainer.h"
#include "Types/EDBOverlayState.h"

#include "DBEquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlot, Slot, UDBEquipmentItemData*, NewItem);


// Socket configuration for equipment per job
USTRUCT(BlueprintType)
struct FEquipmentSocketConfig
{
    GENERATED_BODY()
    
    // Socket when weapon is drawn (e.g., "MainWeapon_r" for right hand)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
    FName EquippedSocketName;
    
    // Socket when weapon is sheathed (e.g., "MainWeapon_Sheath_Back" for back)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
    FName SheathedSocketName;
    
    // NOTE: Position and rotation are configured on the skeletal mesh asset itself
    // in the editor, not at runtime. This keeps the system simple and performant.
};

// Equipment socket data table row
USTRUCT(BlueprintType)
struct FDBEquipmentSocketData : public FTableRowBase
{
    GENERATED_BODY()
    
    // Job tag - e.g., "Job.Paladin"
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FGameplayTag JobTag;
    
    // Equipment slot tag - e.g., "Equipment.Slot.MainWeapon"
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FGameplayTag EquipmentSlotTag;
    
    // Socket configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets")
    FEquipmentSocketConfig SocketConfig;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UDBEquipmentComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Sockets")
    UDataTable* EquipmentSocketDataTable;

    // In the public section, add these method declarations:

    // Get current job as gameplay tag
    FGameplayTag GetCurrentJobTag() const;

    void SetWeaponTypeInABP(UDBWeaponAnimationSet* WeaponData);

    // Get equipment slot as gameplay tag  
    FGameplayTag GetEquipmentSlotAsTag(EEquipmentSlot Slot) const;

    // Get socket configuration for a slot from data table
    FEquipmentSocketConfig* GetSocketConfigForSlot(EEquipmentSlot Slot);
    
    // Equip/Unequip
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool EquipItem(UDBEquipmentItemData* Item);
    
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool UnequipSlot(EEquipmentSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void MoveEquipmentToSocket(EEquipmentSlot Slot, FName NewSocketName);
    
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    UDBEquipmentItemData* GetEquippedItem(EEquipmentSlot Slot) const;
    
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool IsSlotEmpty(EEquipmentSlot Slot) const;
    
    // Weapon State
    UFUNCTION(BlueprintCallable, Category = "Equipment|Weapon")
    void DrawWeapon();
    
    UFUNCTION(BlueprintCallable, Category = "Equipment|Weapon")
    void SheatheWeapon();
    
    UFUNCTION(BlueprintPure, Category = "Equipment|Weapon")
    bool IsWeaponDrawn() const { return bWeaponDrawn; }

    // Set weapon drawn state (called by engage/disengage abilities)
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void SetWeaponDrawn(bool bDrawn);

    // === NEW: DATA-DRIVEN ATTACHMENT METHODS ===
    // These are called by Animation Notifies to attach equipment to correct sockets
    
    UFUNCTION(BlueprintCallable, Category = "Equipment|Attachment")
    void AttachEquipmentToSocket(EEquipmentSlot Slot, bool bEquipped);
    
    UFUNCTION(BlueprintCallable, Category = "Equipment|Attachment")
    void AttachEquipmentByTags(FGameplayTag EquipmentSlotTag, FGameplayTag AttachmentStateTag);

    UFUNCTION(BlueprintCallable, Category = "Equipment|Debug")
    void DebugEquipItems(UDBEquipmentItemData* MainHandItem, UDBEquipmentItemData* OffHandItem);

    UFUNCTION(BlueprintCallable, Category = "Equipment|Debug")
    void DebugEquipTestItems();
    
    // Get total stats from all equipment
    UFUNCTION(BlueprintCallable, Category = "Equipment|Stats")
    FEquipmentStats GetTotalEquipmentStats() const;
    
    // Events
    UPROPERTY(BlueprintAssignable, Category = "Equipment")
    FOnEquipmentChanged OnEquipmentChanged;

    // ========================================
    // Overlay State Management
    // ========================================
    
    /** Current overlay state based on equipped items */
    UPROPERTY(BlueprintReadOnly, Category = "Equipment|Animation")
    EDBOverlayState CurrentOverlayState;
    
    /** Get the current overlay state */
    UFUNCTION(BlueprintPure, Category = "Equipment|Animation")
    EDBOverlayState GetOverlayState() const { return CurrentOverlayState; }
    
    /** Update overlay state based on equipped items */
    UFUNCTION(BlueprintCallable, Category = "Equipment|Animation")
    void UpdateOverlayState();

    void UpdateWeaponVisualState();
    
protected:
    virtual void BeginPlay() override;

    EDBOverlayState DetermineOverlayStateFromEquipment() const;

    FName GetSocketForSlot(EEquipmentSlot Slot);

    bool CanEquipItem(UDBEquipmentItemData* Item) const;
    void ApplyEquipmentStatsForItem(UDBEquipmentItemData* Item);
    void RemoveEquipmentStatsForItem(UDBEquipmentItemData* Item);
    FName GetSocketNameForSlot(EEquipmentSlot Slot) const;

    USkeletalMeshComponent* GetBodyMesh() const;
    
    // Apply stats to GAS
    void ApplyEquipmentStats();
    void RemoveEquipmentStats();

    // Apply or Remove gameplay tags to owners ASC
    void ApplyEquipmentTags(UDBEquipmentItemData* Item);
    void RemoveEquipmentTags(UDBEquipmentItemData* Item);
    
    // Spawn visual meshes
    void UpdateEquipmentVisuals(EEquipmentSlot Slot);

    UPROPERTY(EditDefaultsOnly, Category = "Equipment|Animation")
    UAnimMontage* DrawWeaponMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = "Equipment|Animation")
    UAnimMontage* SheatheWeaponMontage;

    UPROPERTY()
    TMap<EEquipmentSlot, FGameplayTagContainer> EquippedItemTags;

    // Store active equipment effect handles so we can remove them
    UPROPERTY()
    TMap<EEquipmentSlot, FActiveGameplayEffectHandle> ActiveEquipmentEffects;
    
    // Equipment stat GameplayEffect class
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TSubclassOf<UGameplayEffect> EquipmentStatEffectClass;
    
    UPROPERTY()
    TMap<EEquipmentSlot, UDBEquipmentItemData*> EquippedItems;
    
    UPROPERTY()
    TMap<EEquipmentSlot, USkeletalMeshComponent*> SpawnedMeshes;
    
    UPROPERTY()
    bool bWeaponDrawn = false;
    
    UPROPERTY()
    class UAbilitySystemComponent* ASC;
};