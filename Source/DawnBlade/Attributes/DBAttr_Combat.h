// DBAttr_Combat.h
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_Combat.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DAWNBLADE_API UDBAttr_Combat : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UDBAttr_Combat();
    
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // ========================================
    // OFFENSIVE STATS
    // ========================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_Attack)
    FGameplayAttributeData Attack;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Attack)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_RangedAttack)
    FGameplayAttributeData RangedAttack;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, RangedAttack)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_Accuracy)
    FGameplayAttributeData Accuracy;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Accuracy)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_RangedAccuracy)
    FGameplayAttributeData RangedAccuracy;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, RangedAccuracy)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_CriticalHitRate)
    FGameplayAttributeData CriticalHitRate;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, CriticalHitRate)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_CriticalHitDamage)
    FGameplayAttributeData CriticalHitDamage;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, CriticalHitDamage)

    /** TP (Tactical Points) - Used for weapon skills (FFXI mechanic) */
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_TP)
    FGameplayAttributeData TP;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, TP)
    
    /** Maximum TP - Default 3000 (allows 3 weapon skills) */
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Offense", ReplicatedUsing = OnRep_MaxTP)
    FGameplayAttributeData MaxTP;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MaxTP)
    
    // ========================================
    // DEFENSIVE STATS
    // ========================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_Defense)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Defense)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_Evasion)
    FGameplayAttributeData Evasion;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Evasion)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_MagicEvasion)
    FGameplayAttributeData MagicEvasion;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicEvasion)
    
    // ========================================
    // MAGIC STATS
    // ========================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Magic", ReplicatedUsing = OnRep_MagicAccuracy)
    FGameplayAttributeData MagicAccuracy;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicAccuracy)

    /** Magic Attack - Increases magic damage output */
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Magic", ReplicatedUsing = OnRep_MagicAttack)
    FGameplayAttributeData MagicAttack;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicAttack)

    /** Magic Defense - Reduces magic damage taken */
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Magic", ReplicatedUsing = OnRep_MagicDefense)
    FGameplayAttributeData MagicDefense;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicDefense)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Magic", ReplicatedUsing = OnRep_CurePotency)
    FGameplayAttributeData CurePotency;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, CurePotency)
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Magic", ReplicatedUsing = OnRep_PotencyOfCureEffectReceived)
    FGameplayAttributeData PotencyOfCureEffectReceived;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, PotencyOfCureEffectReceived)
    
    // ========================================
    // THREAT
    // ========================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Threat", ReplicatedUsing = OnRep_Enmity)
    FGameplayAttributeData Enmity;
    ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Enmity)
    
protected:
    // ========================================
    // REPLICATION CALLBACKS - OFFENSE
    // ========================================
    
    UFUNCTION() virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);
    UFUNCTION() virtual void OnRep_RangedAttack(const FGameplayAttributeData& OldRangedAttack);
    UFUNCTION() virtual void OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy);
    UFUNCTION() virtual void OnRep_RangedAccuracy(const FGameplayAttributeData& OldRangedAccuracy);
    UFUNCTION() virtual void OnRep_CriticalHitRate(const FGameplayAttributeData& OldCriticalHitRate);
    UFUNCTION() virtual void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage);
    UFUNCTION() virtual void OnRep_TP(const FGameplayAttributeData& OldTP);
    UFUNCTION() virtual void OnRep_MaxTP(const FGameplayAttributeData& OldMaxTP);
    
    // ========================================
    // REPLICATION CALLBACKS - DEFENSE
    // ========================================
    
    UFUNCTION() virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);
    UFUNCTION() virtual void OnRep_Evasion(const FGameplayAttributeData& OldEvasion);
    UFUNCTION() virtual void OnRep_MagicEvasion(const FGameplayAttributeData& OldMagicEvasion);
    
    // ========================================
    // REPLICATION CALLBACKS - MAGIC
    // ========================================
    
    UFUNCTION() virtual void OnRep_MagicAccuracy(const FGameplayAttributeData& OldMagicAccuracy);
    UFUNCTION() virtual void OnRep_MagicAttack(const FGameplayAttributeData& OldMagicAttack);
    UFUNCTION() virtual void OnRep_MagicDefense(const FGameplayAttributeData& OldMagicDefense);
    UFUNCTION() virtual void OnRep_CurePotency(const FGameplayAttributeData& OldCurePotency);
    UFUNCTION() virtual void OnRep_PotencyOfCureEffectReceived(const FGameplayAttributeData& OldPotencyOfCureEffectReceived);
    
    // ========================================
    // REPLICATION CALLBACKS - THREAT
    // ========================================
    
    UFUNCTION() virtual void OnRep_Enmity(const FGameplayAttributeData& OldEnmity);
};