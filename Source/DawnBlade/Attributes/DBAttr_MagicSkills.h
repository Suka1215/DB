// DBAttr_MagicSkills.h - Magic skill levels
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_MagicSkills.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_MagicSkills : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_MagicSkills();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // === MAGIC SKILLS ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_DivineMagicSkill)
    FGameplayAttributeData DivineMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, DivineMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_HealingMagicSkill)
    FGameplayAttributeData HealingMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, HealingMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_EnhancingMagicSkill)
    FGameplayAttributeData EnhancingMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, EnhancingMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_EnfeeblingMagicSkill)
    FGameplayAttributeData EnfeeblingMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, EnfeeblingMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_ElementalMagicSkill)
    FGameplayAttributeData ElementalMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, ElementalMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_DarkMagicSkill)
    FGameplayAttributeData DarkMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, DarkMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_SummoningMagicSkill)
    FGameplayAttributeData SummoningMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, SummoningMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_NinjutsuSkill)
    FGameplayAttributeData NinjutsuSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, NinjutsuSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_SingingSkill)
    FGameplayAttributeData SingingSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, SingingSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_StringInstrumentSkill)
    FGameplayAttributeData StringInstrumentSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, StringInstrumentSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_WindInstrumentSkill)
    FGameplayAttributeData WindInstrumentSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, WindInstrumentSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_BlueMagicSkill)
    FGameplayAttributeData BlueMagicSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, BlueMagicSkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_GeomancySkill)
    FGameplayAttributeData GeomancySkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, GeomancySkill)
    
    UPROPERTY(BlueprintReadOnly, Category = "Magic Skills", ReplicatedUsing = OnRep_HandbellSkill)
    FGameplayAttributeData HandbellSkill;
    ATTRIBUTE_ACCESSORS(UDBAttr_MagicSkills, HandbellSkill)
    
protected:
    UFUNCTION() virtual void OnRep_DivineMagicSkill(const FGameplayAttributeData& OldDivineMagicSkill);
    UFUNCTION() virtual void OnRep_HealingMagicSkill(const FGameplayAttributeData& OldHealingMagicSkill);
    UFUNCTION() virtual void OnRep_EnhancingMagicSkill(const FGameplayAttributeData& OldEnhancingMagicSkill);
    UFUNCTION() virtual void OnRep_EnfeeblingMagicSkill(const FGameplayAttributeData& OldEnfeeblingMagicSkill);
    UFUNCTION() virtual void OnRep_ElementalMagicSkill(const FGameplayAttributeData& OldElementalMagicSkill);
    UFUNCTION() virtual void OnRep_DarkMagicSkill(const FGameplayAttributeData& OldDarkMagicSkill);
    UFUNCTION() virtual void OnRep_SummoningMagicSkill(const FGameplayAttributeData& OldSummoningMagicSkill);
    UFUNCTION() virtual void OnRep_NinjutsuSkill(const FGameplayAttributeData& OldNinjutsuSkill);
    UFUNCTION() virtual void OnRep_SingingSkill(const FGameplayAttributeData& OldSingingSkill);
    UFUNCTION() virtual void OnRep_StringInstrumentSkill(const FGameplayAttributeData& OldStringInstrumentSkill);
    UFUNCTION() virtual void OnRep_WindInstrumentSkill(const FGameplayAttributeData& OldWindInstrumentSkill);
    UFUNCTION() virtual void OnRep_BlueMagicSkill(const FGameplayAttributeData& OldBlueMagicSkill);
    UFUNCTION() virtual void OnRep_GeomancySkill(const FGameplayAttributeData& OldGeomancySkill);
    UFUNCTION() virtual void OnRep_HandbellSkill(const FGameplayAttributeData& OldHandbellSkill);
};