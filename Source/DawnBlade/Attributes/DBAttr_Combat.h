// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "DBAttr_Combat.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBAttr_Combat : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData Attack;
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData Defense;
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData MagicAttack;
	UPROPERTY(BlueprintReadOnly) FGameplayAttributeData MagicDefense;

	ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Attack)
	ATTRIBUTE_ACCESSORS(UDBAttr_Combat, Defense)
	ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicAttack)
	ATTRIBUTE_ACCESSORS(UDBAttr_Combat, MagicDefense)
};
