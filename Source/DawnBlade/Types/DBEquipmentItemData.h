// DBEquipmentItemData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DBEquipmentItemData.generated.h"

class UDBWeaponAnimationSet;

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    // Weapons
    MainHand,
    OffHand,
    Ranged,
    Ammo,
    
    // Armor
    Head,
    Body,
    Hands,
    Legs,
    Feet,
    
    // Accessories
    Back,
    Waist,
    Neck,
    Ring1,
    Ring2,
    Earring1,
    Earring2
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None,
    Sword,
    SwordAndShield,
    GreatSword,
    Axe,
    GreatAxe,
    Dagger,
    Club,
    Staff,
    Polearm,
    Katana,
    GreatKatana,
    Scythe,
    HandToHand,
    Shield,
    Bow,
    Crossbow,
    Gun,
    Thrown
};

USTRUCT(BlueprintType)
struct FEquipmentStats
{
    GENERATED_BODY()
    
    // Primary Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Strength = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Dexterity = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Vitality = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Agility = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Intelligence = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Mind = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Primary Stats")
    int32 Charisma = 0;
    
    // Combat Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 Attack = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 Defense = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 Accuracy = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 Evasion = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 MagicAccuracy = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
    int32 MagicEvasion = 0;
    
    // Resistances
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 FireResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 IceResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 WindResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 EarthResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 LightningResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 WaterResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 LightResistance = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances")
    int32 DarkResistance = 0;
};

UCLASS(BlueprintType)
class DAWNBLADE_API UDBEquipmentItemData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    // Basic Info
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    FText ItemName;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (MultiLine = true))
    FText Description;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EEquipmentSlot EquipmentSlot;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EWeaponType WeaponType;
    
    // Job Restrictions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Requirements")
    FGameplayTagContainer RequiredJobs;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Requirements")
    int32 RequiredLevel = 1;
    
    // Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Stats")
    FEquipmentStats Stats;
    
    // Visual
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Visual")
    TSoftObjectPtr<USkeletalMesh> EquipmentMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Visual")
    TSoftObjectPtr<UStaticMesh> EquipmentStaticMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UDBWeaponAnimationSet* WeaponAnimationSet;
    
    // Animation Tags (NOT Animation Blueprint!)
    // These tags tell the ABP which animation set to use
    // Examples: "Weapon.Type.Sword", "Weapon.Type.Shield", "Weapon.Equipped.SwordAndShield"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Animation")
    FGameplayTagContainer AnimationTags;
};