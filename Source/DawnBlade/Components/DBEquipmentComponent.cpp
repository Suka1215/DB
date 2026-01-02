// DBEquipmentComponent.cpp
#include "DBEquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "DBJobComponent.h"
#include "DBWeaponComponent.h"
#include "GameplayEffect.h" 
#include "Attributes/DBAttr_Combat.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Resistance.h"
#include "GameFramework/Character.h"

UDBEquipmentComponent::UDBEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentOverlayState = EDBOverlayState::Default;
}

void UDBEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Get ASC from owner
    AActor* Owner = GetOwner();
    if (Owner)
    {
        ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
        
        // Initialize weapon as sheathed
        if (ASC)
        {
            ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed")));
        }
    }
}

void UDBEquipmentComponent::DebugEquipItems(UDBEquipmentItemData* MainHandItem, UDBEquipmentItemData* OffHandItem)
{
    if (MainHandItem)
    {
        bool bSuccess = EquipItem(MainHandItem);
        UE_LOG(LogTemp, Warning, TEXT("✅ DEBUG: Equipped %s - Success: %s"), 
            *MainHandItem->ItemName.ToString(), bSuccess ? TEXT("Yes") : TEXT("No"));
    }
    
    if (OffHandItem)
    {
        bool bSuccess = EquipItem(OffHandItem);
        UE_LOG(LogTemp, Warning, TEXT("✅ DEBUG: Equipped %s - Success: %s"), 
            *OffHandItem->ItemName.ToString(), bSuccess ? TEXT("Yes") : TEXT("No"));
    }
    
    // Print total stats
    FEquipmentStats Total = GetTotalEquipmentStats();
    UE_LOG(LogTemp, Warning, TEXT("📊 Total Equipment Stats: ATK=%d DEF=%d STR=%d VIT=%d ACC=%d"), 
        Total.Attack, Total.Defense, Total.Strength, Total.Vitality, Total.Accuracy);
    
    // Print active tags
    if (ASC)
    {
        FGameplayTagContainer OwnedTags;
        ASC->GetOwnedGameplayTags(OwnedTags);
        UE_LOG(LogTemp, Warning, TEXT("🏷️ Active Tags: %s"), *OwnedTags.ToStringSimple());
    }
}

void UDBEquipmentComponent::DebugEquipTestItems()
{
    // Use a timer to delay equipment spawn until character is fully initialized
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        // Load test items with your actual paths
        UDBEquipmentItemData* Sword = Cast<UDBEquipmentItemData>(
            StaticLoadObject(UDBEquipmentItemData::StaticClass(), nullptr, 
            TEXT("/Game/Game/Data/DA_IronSword_Lvl1")));
        
        UDBEquipmentItemData* Shield = Cast<UDBEquipmentItemData>(
            StaticLoadObject(UDBEquipmentItemData::StaticClass(), nullptr, 
            TEXT("/Game/Game/Data/DA_IronShield_Lvl1")));
        
        if (Sword)
        {
            bool bSuccess = EquipItem(Sword);
            UE_LOG(LogTemp, Warning, TEXT("✅ DEBUG: Equipped Iron Sword - Success: %s"), bSuccess ? TEXT("Yes") : TEXT("No"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ DEBUG: Failed to load Iron Sword - check path!"));
        }
        
        if (Shield)
        {
            bool bSuccess = EquipItem(Shield);
            UE_LOG(LogTemp, Warning, TEXT("✅ DEBUG: Equipped Kite Shield - Success: %s"), bSuccess ? TEXT("Yes") : TEXT("No"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ DEBUG: Failed to load Kite Shield - check path!"));
        }
        
        // Print total stats
        FEquipmentStats TotalStats = GetTotalEquipmentStats();
        UE_LOG(LogTemp, Warning, TEXT("📊 Total Equipment Stats: ATK=%d DEF=%d STR=%d VIT=%d ACC=%d"),
            TotalStats.Attack, TotalStats.Defense, TotalStats.Strength, TotalStats.Vitality, TotalStats.Accuracy);
        
        // Print active tags
        if (ASC)
        {
            FGameplayTagContainer OwnedTags;
            ASC->GetOwnedGameplayTags(OwnedTags);
            
            FString TagString;
            for (const FGameplayTag& Tag : OwnedTags)
            {
                if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon"))))
                {
                    TagString += Tag.ToString() + TEXT(", ");
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("🏷️ Active Equipment Tags: %s"), *TagString);
        }
        
    }, 0.5f, false); // 0.5 second delay
}

USkeletalMeshComponent* UDBEquipmentComponent::GetBodyMesh() const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return nullptr;
    }

    // Primary: Use Character's main mesh (this is CharacterMesh0)
    USkeletalMeshComponent* CharMesh = Character->GetMesh();
    if (CharMesh)
    {
        return CharMesh;
    }

    // Fallback: Search for skeletal meshes
    TArray<USkeletalMeshComponent*> SkeletalMeshes;
    Character->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
    
    // First skeletal mesh is usually the main one
    if (SkeletalMeshes.Num() > 0)
    {
        return SkeletalMeshes[0];
    }

    return nullptr;
}

EDBOverlayState UDBEquipmentComponent::DetermineOverlayStateFromEquipment() const
{
    // Get equipped items using your existing GetEquippedItem method
    UDBEquipmentItemData* MainWeapon = GetEquippedItem(EEquipmentSlot::MainHand);
    UDBEquipmentItemData* OffHand = GetEquippedItem(EEquipmentSlot::OffHand);
    
    // Check if we have weapons equipped
    bool bHasMainWeapon = (MainWeapon != nullptr);
    bool bHasOffhand = (OffHand != nullptr);
    
    // Determine weapon types from tags
    if (bHasMainWeapon && MainWeapon->AnimationTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Type.TwoHanded"))))
    {
        return EDBOverlayState::TwoHanded;
    }
    
    if (bHasMainWeapon && MainWeapon->AnimationTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Type.Bow"))))
    {
        return EDBOverlayState::Bow;
    }
    
    // Check for dual wield (weapon in offhand)
    if (bHasOffhand && OffHand->AnimationTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Type.Sword"))))
    {
        return EDBOverlayState::DualWield;
    }
    
    // Sword + Shield (main weapon + shield in offhand)
    if (bHasMainWeapon && bHasOffhand)
    {
        return EDBOverlayState::SwordAndShield;
    }
    
    // Single weapon, no offhand (default to sword and shield stance)
    if (bHasMainWeapon)
    {
        return EDBOverlayState::SwordAndShield;
    }
    
    // No weapons equipped - unarmed/default
    return EDBOverlayState::Default;
}

void UDBEquipmentComponent::UpdateOverlayState()
{
    EDBOverlayState NewState = DetermineOverlayStateFromEquipment();
    
    if (NewState != CurrentOverlayState)
    {
        CurrentOverlayState = NewState;
        UE_LOG(LogTemp, Warning, TEXT("🎭 Overlay state changed to: %d"), (int32)CurrentOverlayState);
        
        // Optional: Broadcast event if you have one
        // OnOverlayStateChanged.Broadcast(CurrentOverlayState);
    }
}

void UDBEquipmentComponent::MoveEquipmentToSocket(EEquipmentSlot Slot, FName NewSocketName)
{
    if (NewSocketName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ MoveEquipmentToSocket: Socket name is None"));
        return;
    }

    // Find the spawned mesh for this slot
    USkeletalMeshComponent* EquippedMesh = SpawnedMeshes.Contains(Slot) ? SpawnedMeshes[Slot] : nullptr;
    if (!EquippedMesh || !IsValid(EquippedMesh))
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No mesh found for slot: %d"), (int32)Slot);
        return;
    }

    // Get the body mesh using helper function
    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Body mesh found!"));
        return;
    }

    // Check if socket exists
    if (!BodyMesh->DoesSocketExist(NewSocketName))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Socket does not exist: %s"), *NewSocketName.ToString());
        
        // Debug: List available sockets
        TArray<FName> AvailableSockets = BodyMesh->GetAllSocketNames();
        UE_LOG(LogTemp, Error, TEXT("   Available sockets:"));
        for (const FName& Socket : AvailableSockets)
        {
            UE_LOG(LogTemp, Error, TEXT("   - %s"), *Socket.ToString());
        }
        return;
    }

    // Detach first
    EquippedMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    // Reattach to new socket
    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        true
    );
    
    bool bAttached = EquippedMesh->AttachToComponent(BodyMesh, AttachRules, NewSocketName);

    if (bAttached && EquippedMesh->GetAttachParent() == BodyMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Moved equipment to socket: %s (Slot %d)"), 
            *NewSocketName.ToString(), (int32)Slot);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to attach to socket: %s"), *NewSocketName.ToString());
    }
}


bool UDBEquipmentComponent::EquipItem(UDBEquipmentItemData* Item)
{
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipItem: Item is null"));
        return false;
    }
    
    // Check if player meets requirements
    if (!CanEquipItem(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipItem: Cannot equip %s - requirements not met"), *Item->ItemName.ToString());
        return false;
    }
    
    EEquipmentSlot Slot = Item->EquipmentSlot;
    
    // Unequip existing item in slot
    if (EquippedItems.Contains(Slot) && EquippedItems[Slot])
    {
        UnequipSlot(Slot);
    }
    
    // Special case: Two-handed weapons unequip offhand
    if (Item->WeaponType == EWeaponType::GreatSword || 
        Item->WeaponType == EWeaponType::GreatAxe || 
        Item->WeaponType == EWeaponType::GreatKatana ||
        Item->WeaponType == EWeaponType::Polearm ||
        Item->WeaponType == EWeaponType::Staff ||
        Item->WeaponType == EWeaponType::Scythe)
    {
        UnequipSlot(EEquipmentSlot::OffHand);
    }
    
    // Equip new item
    EquippedItems.Add(Slot, Item);
    
    // Apply stats
    ApplyEquipmentStatsForItem(Item);
    ApplyEquipmentTags(Item);
    
    // CRITICAL FIX: Notify Weapon Component FIRST (before UpdateEquipmentVisuals)
    if (Slot == EEquipmentSlot::MainHand && Item->WeaponAnimationSet)
    {
        UDBWeaponComponent* WeaponComp = GetOwner()->FindComponentByClass<UDBWeaponComponent>();
        if (WeaponComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("📢 Equipment notifying Weapon Component: %s"), 
                *Item->WeaponAnimationSet->WeaponTypeName.ToString());
            
            WeaponComp->EquipWeapon(Item->WeaponAnimationSet);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ No Weapon Component found to notify"));
        }

        SetWeaponTypeInABP(Item->WeaponAnimationSet);
    }
    
    // NOW update visuals - WeaponComp has data and can provide socket names
    UpdateEquipmentVisuals(Slot);
    
    // Broadcast event
    OnEquipmentChanged.Broadcast(Slot, Item);
    
    UE_LOG(LogTemp, Log, TEXT("Equipped %s in slot %d"), *Item->ItemName.ToString(), (int32)Slot);

    UpdateOverlayState();
    
    return true;
}

void UDBEquipmentComponent::SetWeaponTypeInABP(UDBWeaponAnimationSet* WeaponData)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    USkeletalMeshComponent* BodyMesh = Character->GetMesh();
    if (!BodyMesh) return;

    UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
    if (!AnimInstance) return;

    UE_LOG(LogTemp, Warning, TEXT("🎨 Current ABP: %s"), *AnimInstance->GetClass()->GetName());

    // Convert weapon data to enum value
    int32 WeaponTypeValue = 0; // None/Unarmed
    
    if (WeaponData)
    {
        FString WeaponTypeName = WeaponData->WeaponTypeName.ToString();
        
        // Map to your EWeaponType enum values
        if (WeaponTypeName.Contains("Staff"))
        {
            WeaponTypeValue = 8; // Staff
        }
        else if (WeaponTypeName == "SwordAndShield" || WeaponTypeName.Contains("Shield"))
        {
            WeaponTypeValue = 2; // SwordAndShield
        }
        else if (WeaponTypeName.Contains("GreatSword"))
        {
            WeaponTypeValue = 3; // GreatSword
        }
        else if (WeaponTypeName.Contains("GreatAxe"))
        {
            WeaponTypeValue = 5; // GreatAxe
        }
        else if (WeaponTypeName.Contains("GreatKatana"))
        {
            WeaponTypeValue = 11; // GreatKatana
        }
        else if (WeaponTypeName.Contains("Sword"))
        {
            WeaponTypeValue = 1; // Sword
        }
        else if (WeaponTypeName.Contains("Axe"))
        {
            WeaponTypeValue = 4; // Axe
        }
        else if (WeaponTypeName.Contains("Dagger"))
        {
            WeaponTypeValue = 6; // Dagger
        }
        else if (WeaponTypeName.Contains("Club"))
        {
            WeaponTypeValue = 7; // Club
        }
        else if (WeaponTypeName.Contains("Polearm"))
        {
            WeaponTypeValue = 9; // Polearm
        }
        else if (WeaponTypeName.Contains("Katana"))
        {
            WeaponTypeValue = 10; // Katana
        }
        else if (WeaponTypeName.Contains("Scythe"))
        {
            WeaponTypeValue = 12; // Scythe
        }
        else if (WeaponTypeName.Contains("HandToHand") || WeaponTypeName.Contains("Unarmed"))
        {
            WeaponTypeValue = 13; // HandToHand
        }
        else if (WeaponTypeName.Contains("Bow"))
        {
            WeaponTypeValue = 15; // Bow
        }
        else if (WeaponTypeName.Contains("Crossbow"))
        {
            WeaponTypeValue = 16; // Crossbow
        }
        else if (WeaponTypeName.Contains("Gun"))
        {
            WeaponTypeValue = 17; // Gun
        }
        else if (WeaponTypeName.Contains("Thrown"))
        {
            WeaponTypeValue = 18; // Thrown
        }

        UE_LOG(LogTemp, Warning, TEXT("🎨 Setting ABP Weapon Type to: %d (%s)"), 
            WeaponTypeValue, *WeaponTypeName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("🎨 Setting ABP Weapon Type to: 0 (None/Unarmed)"));
    }

    // Find and set the property by name
    FProperty* WeaponTypeProperty = AnimInstance->GetClass()->FindPropertyByName(FName("WeaponType"));
    
    // Try with space if no space version doesn't work
    if (!WeaponTypeProperty)
    {
        WeaponTypeProperty = AnimInstance->GetClass()->FindPropertyByName(FName("Weapon Type"));
    }
    
    if (WeaponTypeProperty)
    {
        FEnumProperty* EnumProp = CastField<FEnumProperty>(WeaponTypeProperty);
        if (EnumProp)
        {
            void* PropertyValuePtr = EnumProp->ContainerPtrToValuePtr<void>(AnimInstance);
            
            // FIX: Cast to int64 to resolve ambiguity
            EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(PropertyValuePtr, static_cast<int64>(WeaponTypeValue));
            
            UE_LOG(LogTemp, Warning, TEXT("✅ Weapon Type set successfully!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Weapon Type is not an enum property!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not find 'WeaponType' or 'Weapon Type' property in ABP"));
        
        // Debug: List all properties
        UE_LOG(LogTemp, Warning, TEXT("📋 Available properties in ABP:"));
        for (TFieldIterator<FProperty> It(AnimInstance->GetClass()); It; ++It)
        {
            UE_LOG(LogTemp, Warning, TEXT("   - %s"), *It->GetName());
        }
    }
}

bool UDBEquipmentComponent::UnequipSlot(EEquipmentSlot Slot)
{
    if (!EquippedItems.Contains(Slot) || !EquippedItems[Slot])
    {
        return false;
    }
    
    UDBEquipmentItemData* Item = EquippedItems[Slot];
    
    // Remove stats
    RemoveEquipmentStatsForItem(Item);
    
    RemoveEquipmentTags(Item);
    
    // Remove visuals
    if (SpawnedMeshes.Contains(Slot) && SpawnedMeshes[Slot])
    {
        SpawnedMeshes[Slot]->DestroyComponent();
        SpawnedMeshes.Remove(Slot);
    }
    
    // Remove from map
    EquippedItems.Remove(Slot);
    
    // Broadcast event
    OnEquipmentChanged.Broadcast(Slot, nullptr);
    
    UE_LOG(LogTemp, Log, TEXT("Unequipped item from slot %d"), (int32)Slot);

    UpdateOverlayState();
    
    return true;
}

UDBEquipmentItemData* UDBEquipmentComponent::GetEquippedItem(EEquipmentSlot Slot) const
{
    if (EquippedItems.Contains(Slot))
    {
        return EquippedItems[Slot];
    }
    return nullptr;
}

bool UDBEquipmentComponent::IsSlotEmpty(EEquipmentSlot Slot) const
{
    return !EquippedItems.Contains(Slot) || EquippedItems[Slot] == nullptr;
}

bool UDBEquipmentComponent::CanEquipItem(UDBEquipmentItemData* Item) const
{
    if (!Item)
    {
        return false;
    }
    
    // TODO: Check level requirement
    // TODO: Check job requirements using RequiredJobs tags
    
    return true;
}

void UDBEquipmentComponent::DrawWeapon()
{
    if (bWeaponDrawn)
    {
        return;
    }
    
    bWeaponDrawn = true;
    
    if (ASC)
    {
        // Add weapon drawn tag
        ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Drawn")));
        ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed")));
        
        UE_LOG(LogTemp, Log, TEXT("⚔️ Weapon Draw Started"));
    }
    
    // Play draw animation montage
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (Character && Character->GetMesh() && DrawWeaponMontage)
    {
        UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DrawWeaponMontage);
            UE_LOG(LogTemp, Warning, TEXT("🎬 Playing draw weapon montage"));
            
            // The animation notifies will call UpdateWeaponVisualState() at the right time
            return;
        }
    }
    
    // No animation montage, just switch immediately
    UE_LOG(LogTemp, Warning, TEXT("⚠️ No draw montage found, switching sockets immediately"));
    UpdateWeaponVisualState();
}

void UDBEquipmentComponent::UpdateWeaponVisualState()
{
    // Re-attach main hand weapon to correct socket (based on bWeaponDrawn state)
    if (EquippedItems.Contains(EEquipmentSlot::MainHand))
    {
        UpdateEquipmentVisuals(EEquipmentSlot::MainHand);
    }
    
    // Re-attach off-hand weapon/shield to correct socket
    if (EquippedItems.Contains(EEquipmentSlot::OffHand))
    {
        UpdateEquipmentVisuals(EEquipmentSlot::OffHand);
    }
    
    UE_LOG(LogTemp, Log, TEXT("🔄 Updated weapon visual state - Drawn: %s"), bWeaponDrawn ? TEXT("Yes") : TEXT("No"));
}

void UDBEquipmentComponent::SetWeaponDrawn(bool bDrawn)
{
    if (bWeaponDrawn == bDrawn)
    {
        return;
    }
    
    bWeaponDrawn = bDrawn;
    
    // Update gameplay tags
    if (ASC)
    {
        if (bWeaponDrawn)
        {
            ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Drawn")));
            ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed")));
        }
        else
        {
            ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Drawn")));
            ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed")));
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Weapon drawn state: %s"), bWeaponDrawn ? TEXT("Drawn") : TEXT("Sheathed"));
}

void UDBEquipmentComponent::AttachEquipmentToSocket(EEquipmentSlot Slot, bool bEquipped)
{
    UE_LOG(LogTemp, Warning, TEXT("=== AttachEquipmentToSocket ==="));
    UE_LOG(LogTemp, Warning, TEXT("Slot: %d, Equipped: %s"), (int32)Slot, bEquipped ? TEXT("Yes") : TEXT("No"));
    
    // Check if item is equipped
    if (!EquippedItems.Contains(Slot) || !EquippedItems[Slot])
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ No item equipped in slot %d"), (int32)Slot);
        return;
    }
    
    // Check if mesh exists
    USkeletalMeshComponent* MeshToAttach = SpawnedMeshes.Contains(Slot) ? SpawnedMeshes[Slot] : nullptr;
    if (!MeshToAttach || !IsValid(MeshToAttach))
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ No mesh component found for slot %d"), (int32)Slot);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Mesh to attach: %s"), *MeshToAttach->GetName());
    
    // Get character
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Owner is not a Character!"));
        return;
    }
    
    // CRITICAL: Find the Body mesh (same as UpdateEquipmentVisuals)
    USkeletalMeshComponent* BodyMesh = nullptr;
    
    TArray<USkeletalMeshComponent*> SkeletalMeshes;
    Character->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
    
    for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
    {
        FString MeshName = MeshComp->GetName();
        if (MeshName.Contains(TEXT("Body")) || MeshName.Contains(TEXT("Torso")))
        {
            BodyMesh = MeshComp;
            break;
        }
    }
    
    // Fallback: mesh with most sockets
    if (!BodyMesh)
    {
        int32 MaxSockets = 0;
        for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
        {
            TArray<FName> Sockets = MeshComp->GetAllSocketNames();
            if (Sockets.Num() > MaxSockets)
            {
                MaxSockets = Sockets.Num();
                BodyMesh = MeshComp;
            }
        }
    }
    
    if (!BodyMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not find Body mesh!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Body mesh: %s"), *BodyMesh->GetName());
    
    // Get socket configuration from data table
    FEquipmentSocketConfig* SocketConfig = GetSocketConfigForSlot(Slot);
    if (!SocketConfig)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ No socket config found for slot %d"), (int32)Slot);
        return;
    }
    
    // Get the appropriate socket name based on equipped/sheathed state
    FName SocketName = bEquipped ? SocketConfig->EquippedSocketName : SocketConfig->SheathedSocketName;
    
    UE_LOG(LogTemp, Warning, TEXT("🔌 Target socket: %s"), *SocketName.ToString());
    
    // Check if socket exists
    if (!BodyMesh->DoesSocketExist(SocketName))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Socket '%s' does not exist on Body mesh!"), *SocketName.ToString());
        return;
    }
    
    // Detach first (important!)
    MeshToAttach->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    
    // Attach to Body mesh socket using SnapToTarget rule
    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        true
    );
    
    bool bAttached = MeshToAttach->AttachToComponent(BodyMesh, AttachRules, SocketName);
    
    if (bAttached && MeshToAttach->GetAttachParent() == BodyMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Successfully attached to socket: %s"), *SocketName.ToString());
        UE_LOG(LogTemp, Warning, TEXT("   Parent: %s"), *BodyMesh->GetName());
        UE_LOG(LogTemp, Warning, TEXT("   Socket: %s"), *MeshToAttach->GetAttachSocketName().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Attachment failed!"));
        UE_LOG(LogTemp, Error, TEXT("   AttachToComponent returned: %s"), bAttached ? TEXT("true") : TEXT("false"));
        UE_LOG(LogTemp, Error, TEXT("   Current parent: %s"), 
            MeshToAttach->GetAttachParent() ? *MeshToAttach->GetAttachParent()->GetName() : TEXT("None"));
    }
}


void UDBEquipmentComponent::AttachEquipmentByTags(FGameplayTag EquipmentSlotTag, FGameplayTag AttachmentStateTag)
{
    // Convert slot tag to enum
    EEquipmentSlot Slot = EEquipmentSlot::MainHand; // Default
    
    if (EquipmentSlotTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.MainWeapon"))))
    {
        Slot = EEquipmentSlot::MainHand;
    }
    else if (EquipmentSlotTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Shield"))))
    {
        Slot = EEquipmentSlot::OffHand;
    }
    else if (EquipmentSlotTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Head"))))
    {
        Slot = EEquipmentSlot::Head;
    }
    else if (EquipmentSlotTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Body"))))
    {
        Slot = EEquipmentSlot::Body;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttachEquipmentByTags: Unknown slot tag %s"), *EquipmentSlotTag.ToString());
        return;
    }
    
    // Determine if equipped (in hand) or sheathed (on back)
    bool bEquipped = AttachmentStateTag.MatchesTagExact(
        FGameplayTag::RequestGameplayTag(TEXT("Equipment.State.Equipped"))
    );
    
    // Attach to appropriate socket
    AttachEquipmentToSocket(Slot, bEquipped);
}

FEquipmentSocketConfig* UDBEquipmentComponent::GetSocketConfigForSlot(EEquipmentSlot Slot)
{
    if (!EquipmentSocketDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSocketConfigForSlot: No data table set!"));
        return nullptr;
    }
    
    FGameplayTag JobTag = GetCurrentJobTag();
    if (!JobTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSocketConfigForSlot: Invalid job tag"));
        return nullptr;
    }
    
    FGameplayTag SlotTag = GetEquipmentSlotAsTag(Slot);
    if (!SlotTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSocketConfigForSlot: Invalid slot tag"));
        return nullptr;
    }
    
    // Search through all rows in the data table
    TArray<FDBEquipmentSocketData*> AllRows;
    EquipmentSocketDataTable->GetAllRows<FDBEquipmentSocketData>(TEXT("GetSocketConfigForSlot"), AllRows);
    
    for (FDBEquipmentSocketData* Row : AllRows)
    {
        if (Row && Row->JobTag == JobTag && Row->EquipmentSlotTag == SlotTag)
        {
            UE_LOG(LogTemp, Log, TEXT("✅ Found socket config: Job=%s, Slot=%s"), 
                *JobTag.ToString(), *SlotTag.ToString());
            return &Row->SocketConfig;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("⚠️ No socket config found for Job=%s, Slot=%s"), 
        *JobTag.ToString(), *SlotTag.ToString());
    return nullptr;
}

FGameplayTag UDBEquipmentComponent::GetEquipmentSlotAsTag(EEquipmentSlot Slot) const
{
    switch (Slot)
    {
    case EEquipmentSlot::MainHand:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.MainWeapon"));
    case EEquipmentSlot::OffHand:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Shield"));
    case EEquipmentSlot::Head:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Head"));
    case EEquipmentSlot::Body:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Body"));
    case EEquipmentSlot::Hands:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Hands"));
    case EEquipmentSlot::Legs:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Legs"));
    case EEquipmentSlot::Feet:
        return FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Feet"));
    default:
        return FGameplayTag();
    }
}

FGameplayTag UDBEquipmentComponent::GetCurrentJobTag() const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return FGameplayTag();
    }
    
    UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>();
    if (!JobComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetCurrentJobTag: No JobComponent found"));
        return FGameplayTag();
    }
    
    // Get the enum job from your JobComponent
    EDBJob CurrentJob = JobComp->GetCurrentJob();
    
    // Convert enum to gameplay tag
    switch (CurrentJob)
    {
    case EDBJob::Warrior:
        return FGameplayTag::RequestGameplayTag(TEXT("Job.Warrior"));
            
    case EDBJob::LightMage:
        return FGameplayTag::RequestGameplayTag(TEXT("Job.LightMage"));
            
    case EDBJob::Paladin:
        return FGameplayTag::RequestGameplayTag(TEXT("Job.Paladin"));
            
    case EDBJob::None:
    default:
        UE_LOG(LogTemp, Warning, TEXT("GetCurrentJobTag: Unknown or None job"));
        return FGameplayTag();
    }
}

void UDBEquipmentComponent::SheatheWeapon()
{
    if (!bWeaponDrawn)
    {
        return;
    }
    
    bWeaponDrawn = false;
    
    if (ASC)
    {
        // Add weapon sheathed tag
        ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Drawn")));
        ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed")));
        
        UE_LOG(LogTemp, Log, TEXT("Weapon Sheathed - Added Weapon.Sheathed tag"));
    }
    
    // TODO: Play sheathe animation montage
}

FEquipmentStats UDBEquipmentComponent::GetTotalEquipmentStats() const
{
    FEquipmentStats TotalStats;
    
    for (const auto& Pair : EquippedItems)
    {
        if (Pair.Value)
        {
            const FEquipmentStats& ItemStats = Pair.Value->Stats;
            
            // Add all stats
            TotalStats.Strength += ItemStats.Strength;
            TotalStats.Dexterity += ItemStats.Dexterity;
            TotalStats.Vitality += ItemStats.Vitality;
            TotalStats.Agility += ItemStats.Agility;
            TotalStats.Intelligence += ItemStats.Intelligence;
            TotalStats.Mind += ItemStats.Mind;
            TotalStats.Charisma += ItemStats.Charisma;
            
            TotalStats.Attack += ItemStats.Attack;
            TotalStats.Defense += ItemStats.Defense;
            TotalStats.Accuracy += ItemStats.Accuracy;
            TotalStats.Evasion += ItemStats.Evasion;
            TotalStats.MagicAccuracy += ItemStats.MagicAccuracy;
            TotalStats.MagicEvasion += ItemStats.MagicEvasion;
            
            TotalStats.FireResistance += ItemStats.FireResistance;
            TotalStats.IceResistance += ItemStats.IceResistance;
            TotalStats.WindResistance += ItemStats.WindResistance;
            TotalStats.EarthResistance += ItemStats.EarthResistance;
            TotalStats.LightningResistance += ItemStats.LightningResistance;
            TotalStats.WaterResistance += ItemStats.WaterResistance;
            TotalStats.LightResistance += ItemStats.LightResistance;
            TotalStats.DarkResistance += ItemStats.DarkResistance;
        }
    }
    
    return TotalStats;
}

void UDBEquipmentComponent::ApplyEquipmentStatsForItem(UDBEquipmentItemData* Item)
{
    if (!Item || !ASC)
    {
        return;
    }
    
    const FEquipmentStats& Stats = Item->Stats;
    
    // Create a dynamic instant GameplayEffect to modify attributes
    UGameplayEffect* EquipmentEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("EquipmentEffect")));
    EquipmentEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;
    
    int32 Idx = 0;
    
    // Helper lambda to add modifiers
    auto AddModifier = [&](const FGameplayAttribute& Attribute, float Value)
    {
        if (Value != 0)
        {
            FGameplayModifierInfo& ModInfo = EquipmentEffect->Modifiers.AddDefaulted_GetRef();
            ModInfo.ModifierMagnitude = FScalableFloat(Value);
            ModInfo.ModifierOp = EGameplayModOp::Additive;
            ModInfo.Attribute = Attribute;
        }
    };
    
    // Add Primary Stat Modifiers
    AddModifier(UDBAttr_Primary::GetStrengthAttribute(), Stats.Strength);
    AddModifier(UDBAttr_Primary::GetDexterityAttribute(), Stats.Dexterity);
    AddModifier(UDBAttr_Primary::GetVitalityAttribute(), Stats.Vitality);
    AddModifier(UDBAttr_Primary::GetAgilityAttribute(), Stats.Agility);
    AddModifier(UDBAttr_Primary::GetIntelligenceAttribute(), Stats.Intelligence);
    AddModifier(UDBAttr_Primary::GetMindAttribute(), Stats.Mind);
    AddModifier(UDBAttr_Primary::GetCharismaAttribute(), Stats.Charisma);
    
    // Add Combat Stat Modifiers
    AddModifier(UDBAttr_Combat::GetAttackAttribute(), Stats.Attack);
    AddModifier(UDBAttr_Combat::GetDefenseAttribute(), Stats.Defense);
    AddModifier(UDBAttr_Combat::GetAccuracyAttribute(), Stats.Accuracy);
    AddModifier(UDBAttr_Combat::GetEvasionAttribute(), Stats.Evasion);
    AddModifier(UDBAttr_Combat::GetMagicAccuracyAttribute(), Stats.MagicAccuracy);
    AddModifier(UDBAttr_Combat::GetMagicEvasionAttribute(), Stats.MagicEvasion);
    
    // Add Resistance Modifiers
    AddModifier(UDBAttr_Resistance::GetFireResistanceAttribute(), Stats.FireResistance);
    AddModifier(UDBAttr_Resistance::GetIceResistanceAttribute(), Stats.IceResistance);
    AddModifier(UDBAttr_Resistance::GetWindResistanceAttribute(), Stats.WindResistance);
    AddModifier(UDBAttr_Resistance::GetEarthResistanceAttribute(), Stats.EarthResistance);
    AddModifier(UDBAttr_Resistance::GetLightningResistanceAttribute(), Stats.LightningResistance);
    AddModifier(UDBAttr_Resistance::GetWaterResistanceAttribute(), Stats.WaterResistance);
    AddModifier(UDBAttr_Resistance::GetLightResistanceAttribute(), Stats.LightResistance);
    AddModifier(UDBAttr_Resistance::GetDarkResistanceAttribute(), Stats.DarkResistance);
    
    // Apply the effect
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);
    
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EquipmentEffect->GetClass(), 1, EffectContext);
    if (SpecHandle.IsValid())
    {
        FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        ActiveEquipmentEffects.Add(Item->EquipmentSlot, ActiveHandle);
        
        UE_LOG(LogTemp, Log, TEXT("Applied equipment stats from %s: STR+%d DEX+%d VIT+%d ATK+%d DEF+%d"), 
            *Item->ItemName.ToString(),
            Stats.Strength,
            Stats.Dexterity,
            Stats.Vitality,
            Stats.Attack,
            Stats.Defense);
    }
}

void UDBEquipmentComponent::ApplyEquipmentTags(UDBEquipmentItemData* Item)
{
    if (!Item || !ASC)
    {
        return;
    }
    
    // Apply animation tags to character
    for (const FGameplayTag& Tag : Item->AnimationTags)
    {
        ASC->AddLooseGameplayTag(Tag);
    }
    
    // Store tags so we can remove them later
    EquippedItemTags.Add(Item->EquipmentSlot, Item->AnimationTags);
    
    UE_LOG(LogTemp, Log, TEXT("Applied equipment tags from %s"), *Item->ItemName.ToString());
}

void UDBEquipmentComponent::RemoveEquipmentTags(UDBEquipmentItemData* Item)
{
    if (!Item || !ASC)
    {
        return;
    }
    
    EEquipmentSlot Slot = Item->EquipmentSlot;
    
    if (EquippedItemTags.Contains(Slot))
    {
        // Remove all tags from this equipment
        for (const FGameplayTag& Tag : EquippedItemTags[Slot])
        {
            ASC->RemoveLooseGameplayTag(Tag);
        }
        
        EquippedItemTags.Remove(Slot);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Removed equipment tags from %s"), *Item->ItemName.ToString());
}

void UDBEquipmentComponent::RemoveEquipmentStatsForItem(UDBEquipmentItemData* Item)
{
    if (!Item || !ASC)
    {
        return;
    }
    
    EEquipmentSlot Slot = Item->EquipmentSlot;
    
    if (ActiveEquipmentEffects.Contains(Slot))
    {
        FActiveGameplayEffectHandle Handle = ActiveEquipmentEffects[Slot];
        ASC->RemoveActiveGameplayEffect(Handle);
        ActiveEquipmentEffects.Remove(Slot);
        
        UE_LOG(LogTemp, Log, TEXT("Removed equipment stats from %s"), *Item->ItemName.ToString());
    }
}

void UDBEquipmentComponent::UpdateEquipmentVisuals(EEquipmentSlot Slot)
{
    UE_LOG(LogTemp, Warning, TEXT("🎨 UpdateEquipmentVisuals for slot %d"), (int32)Slot);
    
    // Check if item exists
    if (!EquippedItems.Contains(Slot) || !EquippedItems[Slot])
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No item in slot %d"), (int32)Slot);
        
        // Remove old mesh if exists
        if (SpawnedMeshes.Contains(Slot))
        {
            if (USkeletalMeshComponent* OldMesh = SpawnedMeshes[Slot])
            {
                UE_LOG(LogTemp, Warning, TEXT("🗑️ Destroying old mesh for slot %d"), (int32)Slot);
                OldMesh->DestroyComponent();
            }
            SpawnedMeshes.Remove(Slot);
        }
        return;
    }
    
    UDBEquipmentItemData* Item = EquippedItems[Slot];
    UE_LOG(LogTemp, Warning, TEXT("✅ Item: %s"), *Item->ItemName.ToString());
    
    // Get character
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Owner is not a Character!"));
        return;
    }
    
    // Get the body mesh using helper function
    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not find Body mesh!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Using mesh: %s"), *BodyMesh->GetName());
    
    // Get socket name from weapon data or use defaults
    FName TargetSocket = GetSocketForSlot(Slot);
    
    if (TargetSocket == NAME_None)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No socket for slot %d"), (int32)Slot);
        return;
    }
    
    // Verify socket exists
    if (!BodyMesh->DoesSocketExist(TargetSocket))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Socket '%s' doesn't exist!"), *TargetSocket.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Available sockets:"));
        TArray<FName> Sockets = BodyMesh->GetAllSocketNames();
        for (const FName& Socket : Sockets)
        {
            UE_LOG(LogTemp, Warning, TEXT("  - %s"), *Socket.ToString());
        }
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("📍 Using socket: %s"), *TargetSocket.ToString());
    
    // Remove old mesh if exists
    if (SpawnedMeshes.Contains(Slot))
    {
        if (USkeletalMeshComponent* OldMesh = SpawnedMeshes[Slot])
        {
            UE_LOG(LogTemp, Warning, TEXT("🗑️ Destroying old mesh"));
            OldMesh->DestroyComponent();
        }
        SpawnedMeshes.Remove(Slot);
    }
    
    // Load mesh
    if (Item->EquipmentMesh.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Item has no EquipmentMesh set!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("📦 Loading mesh: %s"), *Item->EquipmentMesh.ToString());
    
    USkeletalMesh* LoadedMesh = Item->EquipmentMesh.LoadSynchronous();
    if (!LoadedMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to load mesh from path: %s"), *Item->EquipmentMesh.ToString());
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Mesh loaded successfully"));
    
    // Create component
    FString ComponentName = FString::Printf(TEXT("EquipMesh_%s_%d"), 
        *Item->ItemName.ToString().Replace(TEXT(" "), TEXT("_")), (int32)Slot);
    
    UE_LOG(LogTemp, Warning, TEXT("🔨 Creating component: %s"), *ComponentName);
    
    USkeletalMeshComponent* NewMesh = NewObject<USkeletalMeshComponent>(
        Character,
        USkeletalMeshComponent::StaticClass(),
        FName(*ComponentName)
    );
    
    if (!NewMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to create skeletal mesh component!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Component created"));
    
    // Set mesh
    NewMesh->SetSkeletalMesh(LoadedMesh);
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Mesh assigned to component"));
    
    // Setup attachment
    NewMesh->SetupAttachment(BodyMesh, TargetSocket);
    
    // Register
    NewMesh->RegisterComponent();
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Component registered"));
    
    // Attach
    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        true
    );
    
    bool bAttached = NewMesh->AttachToComponent(BodyMesh, AttachRules, TargetSocket);
    
    // Verify attachment
    if (bAttached && NewMesh->GetAttachParent() == BodyMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Attached to %s at socket %s"), 
            *BodyMesh->GetName(), *TargetSocket.ToString());
        
        // CRITICAL: Store in map
        SpawnedMeshes.Add(Slot, NewMesh);
        UE_LOG(LogTemp, Warning, TEXT("✅ Stored in SpawnedMeshes map - Slot %d"), (int32)Slot);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Attachment failed!"));
        UE_LOG(LogTemp, Error, TEXT("   bAttached: %s"), bAttached ? TEXT("true") : TEXT("false"));
        UE_LOG(LogTemp, Error, TEXT("   AttachParent: %s"), 
            NewMesh->GetAttachParent() ? *NewMesh->GetAttachParent()->GetName() : TEXT("None"));
        
        NewMesh->DestroyComponent();
    }
}

FName UDBEquipmentComponent::GetSocketForSlot(EEquipmentSlot Slot)
{
    // For weapons, get socket from Weapon Component
    if (Slot == EEquipmentSlot::MainHand || Slot == EEquipmentSlot::OffHand)
    {
        UDBWeaponComponent* WeaponComp = GetOwner()->FindComponentByClass<UDBWeaponComponent>();
        if (WeaponComp)
        {
            UDBWeaponAnimationSet* WeaponData = WeaponComp->GetCurrentWeaponData();
            if (WeaponData)
            {
                EWeaponState State = WeaponComp->GetWeaponState();
                
                // CRITICAL FIX: Treat Unarmed state as Sheathed for socket selection
                // When equipment first spawns, weapon state is Unarmed but we need sheathed sockets
                bool bDrawn = (State == EWeaponState::Drawn || State == EWeaponState::Engaging);

                if (Slot == EEquipmentSlot::MainHand)
                {
                    FName SocketName = bDrawn ? WeaponData->MainHandDrawnSocket : WeaponData->MainHandSheathedSocket;
                    UE_LOG(LogTemp, Warning, TEXT("🗡️ Main hand socket: %s (%s, State=%d)"), 
                        *SocketName.ToString(), bDrawn ? TEXT("drawn") : TEXT("sheathed"), (int32)State);
                    return SocketName;
                }
                else if (WeaponData->bUsesOffHand)
                {
                    FName SocketName = bDrawn ? WeaponData->OffHandDrawnSocket : WeaponData->OffHandSheathedSocket;
                    UE_LOG(LogTemp, Warning, TEXT("🛡️ Off-hand socket: %s (%s, State=%d)"), 
                        *SocketName.ToString(), bDrawn ? TEXT("drawn") : TEXT("sheathed"), (int32)State);
                    return SocketName;
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("⚠️ No weapon data - using fallback sockets"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠️ No weapon component found - using fallback sockets"));
        }
    }

    // For armor and fallback, use standard sockets
    switch (Slot)
    {
        case EEquipmentSlot::MainHand:  return FName("weapon_r");  // Fallback
        case EEquipmentSlot::OffHand:   return FName("weapon_l");  // Fallback
        case EEquipmentSlot::Ranged:    return FName("weapon_back");
        case EEquipmentSlot::Head:      return FName("head");
        case EEquipmentSlot::Body:      return FName("spine_05");
        case EEquipmentSlot::Hands:     return FName("hand_r");
        case EEquipmentSlot::Legs:      return FName("pelvis");
        case EEquipmentSlot::Feet:      return FName("foot_r");
        case EEquipmentSlot::Back:      return FName("cape_attach");
        default:                        return FName("root");
    }
}

FName UDBEquipmentComponent::GetSocketNameForSlot(EEquipmentSlot Slot) const
{
    switch (Slot)
    {
        case EEquipmentSlot::MainHand:
            return FName("weapon_r");
        case EEquipmentSlot::OffHand:
            return FName("weapon_l");
        case EEquipmentSlot::Ranged:
            return FName("weapon_back");
        case EEquipmentSlot::Back:
            return FName("cape_attach");
        default:
            return FName("root");
    }
}