// DBAttr_Primary.h - Core vital and base attributes
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_Primary.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_Primary : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_Primary();
    
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // === LEVEL ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
    FGameplayAttributeData Level;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Level)
    
    UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_SubjobLevel)
    FGameplayAttributeData SubjobLevel;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, SubjobLevel)
    
    // === VITAL RESOURCES ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Health)
    
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, MaxHealth)
    
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Mana)
    
    UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, MaxMana)
    
    // === CORE ATTRIBUTES ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Strength)
    FGameplayAttributeData Strength;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Strength)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Dexterity)
    FGameplayAttributeData Dexterity;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Dexterity)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Vitality)
    FGameplayAttributeData Vitality;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Vitality)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Agility)
    FGameplayAttributeData Agility;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Agility)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Intelligence)
    FGameplayAttributeData Intelligence;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Intelligence)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Mind)
    FGameplayAttributeData Mind;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Mind)
    
    UPROPERTY(BlueprintReadOnly, Category = "Core Attributes", ReplicatedUsing = OnRep_Charisma)
    FGameplayAttributeData Charisma;
    ATTRIBUTE_ACCESSORS(UDBAttr_Primary, Charisma)
    
protected:
    UFUNCTION() virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);
    UFUNCTION() virtual void OnRep_SubjobLevel(const FGameplayAttributeData& OldSubjobLevel);
    UFUNCTION() virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION() virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION() virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION() virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION() virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);
    UFUNCTION() virtual void OnRep_Dexterity(const FGameplayAttributeData& OldDexterity);
    UFUNCTION() virtual void OnRep_Vitality(const FGameplayAttributeData& OldVitality);
    UFUNCTION() virtual void OnRep_Agility(const FGameplayAttributeData& OldAgility);
    UFUNCTION() virtual void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);
    UFUNCTION() virtual void OnRep_Mind(const FGameplayAttributeData& OldMind);
    UFUNCTION() virtual void OnRep_Charisma(const FGameplayAttributeData& OldCharisma);
};