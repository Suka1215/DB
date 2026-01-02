// DBAttr_Secondary.h - Secondary stats and modifiers
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_Secondary.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_Secondary : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_Secondary();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // === SPEED MODIFIERS ===
    
    // Reduces delay between melee attack rounds and spell recast (percentage)
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Speed", ReplicatedUsing = OnRep_Haste)
    FGameplayAttributeData Haste;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Haste)
    
    // The inverse of Haste (percentage)
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Speed", ReplicatedUsing = OnRep_Slow)
    FGameplayAttributeData Slow;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Slow)
    
    // Movement speed over terrain
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Speed", ReplicatedUsing = OnRep_MovementSpeed)
    FGameplayAttributeData MovementSpeed;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, MovementSpeed)
    
    // === REGENERATION ===
    
    // Gain this amount of HP every tick
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Regeneration", ReplicatedUsing = OnRep_Regen)
    FGameplayAttributeData Regen;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Regen)
    
    // Gain this amount of MP every tick
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Regeneration", ReplicatedUsing = OnRep_Refresh)
    FGameplayAttributeData Refresh;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Refresh)
    
    // Gain this amount of TP every tick
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Regeneration", ReplicatedUsing = OnRep_Regain)
    FGameplayAttributeData Regain;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Regain)
    
    // Add to base HP recovered when using /heal command
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Regeneration", ReplicatedUsing = OnRep_HPRecoveredWhileHealing)
    FGameplayAttributeData HPRecoveredWhileHealing;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, HPRecoveredWhileHealing)
    
    // Add to base MP recovered when using /heal command
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Regeneration", ReplicatedUsing = OnRep_MPRecoveredWhileHealing)
    FGameplayAttributeData MPRecoveredWhileHealing;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, MPRecoveredWhileHealing)
    
    // === DAMAGE MODIFIERS ===
    
    // Modifies damage taken (percentage)
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Damage", ReplicatedUsing = OnRep_DamageTaken)
    FGameplayAttributeData DamageTaken;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, DamageTaken)
    
    // === SPELL CASTING ===
    
    // Modifies base chance of being interrupted by damage (percentage)
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Casting", ReplicatedUsing = OnRep_SpellInterruption)
    FGameplayAttributeData SpellInterruption;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, SpellInterruption)
    
    // Increases spellcasting time and spell recast delay (percentage)
    UPROPERTY(BlueprintReadOnly, Category = "Secondary|Casting", ReplicatedUsing = OnRep_Addle)
    FGameplayAttributeData Addle;
    ATTRIBUTE_ACCESSORS(UDBAttr_Secondary, Addle)
    
protected:
    UFUNCTION() virtual void OnRep_Haste(const FGameplayAttributeData& OldHaste);
    UFUNCTION() virtual void OnRep_Slow(const FGameplayAttributeData& OldSlow);
    UFUNCTION() virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
    UFUNCTION() virtual void OnRep_Regen(const FGameplayAttributeData& OldRegen);
    UFUNCTION() virtual void OnRep_Refresh(const FGameplayAttributeData& OldRefresh);
    UFUNCTION() virtual void OnRep_Regain(const FGameplayAttributeData& OldRegain);
    UFUNCTION() virtual void OnRep_HPRecoveredWhileHealing(const FGameplayAttributeData& OldHPRecoveredWhileHealing);
    UFUNCTION() virtual void OnRep_MPRecoveredWhileHealing(const FGameplayAttributeData& OldMPRecoveredWhileHealing);
    UFUNCTION() virtual void OnRep_DamageTaken(const FGameplayAttributeData& OldDamageTaken);
    UFUNCTION() virtual void OnRep_SpellInterruption(const FGameplayAttributeData& OldSpellInterruption);
    UFUNCTION() virtual void OnRep_Addle(const FGameplayAttributeData& OldAddle);
};