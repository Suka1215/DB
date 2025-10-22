// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_HolySeal.generated.h"

/**
 * Holy Seal - Enhances next heal spell to double potency
 * Instant cast, 15 MP cost, 1 minute cooldown, 10 second window
 */
UCLASS()
class DAWNBLADE_API UDBGA_HolySeal : public UDBGA_Base
{
	GENERATED_BODY()
	
public:
	UDBGA_HolySeal();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Duration of the holy seal effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Holy Seal")
	float SealDuration = 10.f;

	// Healing potency multiplier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Holy Seal")
	float HealingMultiplier = 2.0f;  // Double healing

	// Gameplay Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Holy Seal")
	TSubclassOf<UGameplayEffect> GE_HolySeal;
};
