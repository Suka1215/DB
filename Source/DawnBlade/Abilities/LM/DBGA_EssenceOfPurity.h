// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_EssenceOfPurity.generated.h"

/**
 * Essence of Purity - Active healing boost
 * Enhances next 3 healing spells with +50% potency for 15 seconds
 * Preparation ability for intense healing phases
 */
UCLASS()
class DAWNBLADE_API UDBGA_EssenceOfPurity : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_EssenceOfPurity();

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Healing power enhancement percentage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Essence of Purity")
	float HealingBoostPercentage = 0.15f;  // +15% healing power

	// Gameplay Effect for the passive bonus
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Essence of Purity")
	TSubclassOf<UGameplayEffect> GE_EssenceOfPurity;

private:
	// Handle to the active passive effect
	FActiveGameplayEffectHandle PassiveEffectHandle;
    
	// Apply the passive effect
	void ApplyPassiveEffect();
    
	// Remove the passive effect
	void RemovePassiveEffect();
};
