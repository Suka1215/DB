// DBAttr_Resistance.h - Elemental resistances
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_Resistance.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_Resistance : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_Resistance();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // === ELEMENTAL RESISTANCES ===
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_FireResistance)
    FGameplayAttributeData FireResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, FireResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_IceResistance)
    FGameplayAttributeData IceResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, IceResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_WindResistance)
    FGameplayAttributeData WindResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, WindResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_EarthResistance)
    FGameplayAttributeData EarthResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, EarthResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_LightningResistance)
    FGameplayAttributeData LightningResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, LightningResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_WaterResistance)
    FGameplayAttributeData WaterResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, WaterResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_LightResistance)
    FGameplayAttributeData LightResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, LightResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Resistance|Elemental", ReplicatedUsing = OnRep_DarkResistance)
    FGameplayAttributeData DarkResistance;
    ATTRIBUTE_ACCESSORS(UDBAttr_Resistance, DarkResistance)
    
protected:
    UFUNCTION() virtual void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance);
    UFUNCTION() virtual void OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance);
    UFUNCTION() virtual void OnRep_WindResistance(const FGameplayAttributeData& OldWindResistance);
    UFUNCTION() virtual void OnRep_EarthResistance(const FGameplayAttributeData& OldEarthResistance);
    UFUNCTION() virtual void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance);
    UFUNCTION() virtual void OnRep_WaterResistance(const FGameplayAttributeData& OldWaterResistance);
    UFUNCTION() virtual void OnRep_LightResistance(const FGameplayAttributeData& OldLightResistance);
    UFUNCTION() virtual void OnRep_DarkResistance(const FGameplayAttributeData& OldDarkResistance);
};