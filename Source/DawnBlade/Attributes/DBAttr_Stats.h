// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DBAttr_Stats.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBAttr_Stats : public UAttributeSet
{
	GENERATED_BODY()
public:
	UDBAttr_Stats();
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health) FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth) FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana) FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana) FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TP) FGameplayAttributeData TP;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxTP) FGameplayAttributeData MaxTP;

	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, Health)
	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, MaxHealth)
	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, Mana)
	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, MaxMana)
	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, TP)
	ATTRIBUTE_ACCESSORS(UDBAttr_Stats, MaxTP)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, Health, Old); }
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, MaxHealth, Old); }
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, Mana, Old); }
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, MaxMana, Old); }
	UFUNCTION() void OnRep_TP(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, TP, Old); }
	UFUNCTION() void OnRep_MaxTP(const FGameplayAttributeData& Old){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Stats, MaxTP, Old); }
};
