// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_MysticBoon.generated.h"

/**
 * Mystic Boon - Passive ability that enhances healing output
 * Always active, no cost or cooldown (job trait)
 */
UCLASS()
class DAWNBLADE_API UDBGA_MysticBoon : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_MysticBoon();

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Mystic Boon rank (1-5)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mystic Boon", meta = (ClampMin = 1, ClampMax = 5))
	int32 MysticBoonRank = 1;

	// Healing power bonus per rank
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mystic Boon")
	TArray<float> HealingBonusPerRank = {0.10f, 0.15f, 0.20f, 0.25f, 0.30f};  // 10%, 15%, 20%, 25%, 30%

	// Required levels for each rank
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mystic Boon")
	TArray<int32> RequiredLevelsPerRank = {5, 15, 25, 35, 45};

	// Gameplay Effect for passive bonus
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mystic Boon")
	TSubclassOf<UGameplayEffect> GE_MysticBoon;

private:
	// Handle to the active passive effect
	FActiveGameplayEffectHandle PassiveEffectHandle;
    
	// Apply the passive effect
	void ApplyPassiveEffect();
    
	// Remove the passive effect
	void RemovePassiveEffect();
    
	// Get the appropriate rank based on character level
	int32 GetRankForLevel(int32 CharacterLevel) const;
	
};
