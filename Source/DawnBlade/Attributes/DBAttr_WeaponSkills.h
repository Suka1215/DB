// DBAttr_WeaponSkills.h - Combat skills for weapon types
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_WeaponSkills.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_WeaponSkills : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_WeaponSkills();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // === MELEE WEAPON SKILLS ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_SwordSkill)
    FGameplayAttributeData SwordSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, SwordSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_DaggerSkill)
    FGameplayAttributeData DaggerSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, DaggerSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_AxeSkill)
    FGameplayAttributeData AxeSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, AxeSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_ClubSkill)
    FGameplayAttributeData ClubSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ClubSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_StaffSkill)
    FGameplayAttributeData StaffSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, StaffSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_PolearmSkill)
    FGameplayAttributeData PolearmSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, PolearmSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_KatanaSkill)
    FGameplayAttributeData KatanaSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, KatanaSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_GreatSwordSkill)
    FGameplayAttributeData GreatSwordSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, GreatSwordSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_GreatAxeSkill)
    FGameplayAttributeData GreatAxeSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, GreatAxeSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_GreatKatanaSkill)
    FGameplayAttributeData GreatKatanaSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, GreatKatanaSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_ScytheSkill)
    FGameplayAttributeData ScytheSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ScytheSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Melee", ReplicatedUsing = OnRep_HandToHandSkill)
    FGameplayAttributeData HandToHandSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, HandToHandSkill)
    
    // === RANGED WEAPON SKILLS ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Ranged", ReplicatedUsing = OnRep_ArcherySkill)
    FGameplayAttributeData ArcherySkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ArcherySkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Ranged", ReplicatedUsing = OnRep_MarksmanshipSkill)
    FGameplayAttributeData MarksmanshipSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, MarksmanshipSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Ranged", ReplicatedUsing = OnRep_ThrowingSkill)
    FGameplayAttributeData ThrowingSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ThrowingSkill)
    
    // === DEFENSIVE SKILLS ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Defense", ReplicatedUsing = OnRep_ShieldSkill)
    FGameplayAttributeData ShieldSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ShieldSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Defense", ReplicatedUsing = OnRep_ParryingSkill)
    FGameplayAttributeData ParryingSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, ParryingSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Defense", ReplicatedUsing = OnRep_GuardSkill)
    FGameplayAttributeData GuardSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, GuardSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon Skills|Defense", ReplicatedUsing = OnRep_EvasionSkill)
    FGameplayAttributeData EvasionSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_WeaponSkills, EvasionSkill)
    
protected:
    // Rep notifies for all skills
    UFUNCTION() virtual void OnRep_SwordSkill(const FGameplayAttributeData& OldSwordSkill);
    UFUNCTION() virtual void OnRep_DaggerSkill(const FGameplayAttributeData& OldDaggerSkill);
    UFUNCTION() virtual void OnRep_AxeSkill(const FGameplayAttributeData& OldAxeSkill);
    UFUNCTION() virtual void OnRep_ClubSkill(const FGameplayAttributeData& OldClubSkill);
    UFUNCTION() virtual void OnRep_StaffSkill(const FGameplayAttributeData& OldStaffSkill);
    UFUNCTION() virtual void OnRep_PolearmSkill(const FGameplayAttributeData& OldPolearmSkill);
    UFUNCTION() virtual void OnRep_KatanaSkill(const FGameplayAttributeData& OldKatanaSkill);
    UFUNCTION() virtual void OnRep_GreatSwordSkill(const FGameplayAttributeData& OldGreatSwordSkill);
    UFUNCTION() virtual void OnRep_GreatAxeSkill(const FGameplayAttributeData& OldGreatAxeSkill);
    UFUNCTION() virtual void OnRep_GreatKatanaSkill(const FGameplayAttributeData& OldGreatKatanaSkill);
    UFUNCTION() virtual void OnRep_ScytheSkill(const FGameplayAttributeData& OldScytheSkill);
    UFUNCTION() virtual void OnRep_HandToHandSkill(const FGameplayAttributeData& OldHandToHandSkill);
    UFUNCTION() virtual void OnRep_ArcherySkill(const FGameplayAttributeData& OldArcherySkill);
    UFUNCTION() virtual void OnRep_MarksmanshipSkill(const FGameplayAttributeData& OldMarksmanshipSkill);
    UFUNCTION() virtual void OnRep_ThrowingSkill(const FGameplayAttributeData& OldThrowingSkill);
    UFUNCTION() virtual void OnRep_ShieldSkill(const FGameplayAttributeData& OldShieldSkill);
    UFUNCTION() virtual void OnRep_ParryingSkill(const FGameplayAttributeData& OldParryingSkill);
    UFUNCTION() virtual void OnRep_GuardSkill(const FGameplayAttributeData& OldGuardSkill);
    UFUNCTION() virtual void OnRep_EvasionSkill(const FGameplayAttributeData& OldEvasionSkill);
};