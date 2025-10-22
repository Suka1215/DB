// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Embrace.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGA_Embrace : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_Embrace();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// HP percentage to revive with (25% of max HP)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Life's Embrace")
	float ReviveHPPercentage = 0.25f;

	// Death prevention duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Life's Embrace")
	float DeathPreventionDuration = 10.f;

	// Gameplay Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Life's Embrace")
	TSubclassOf<UGameplayEffect> GE_Revival;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Life's Embrace")
	TSubclassOf<UGameplayEffect> GE_DeathPrevention;

private:
	// Check if target is dead or unconscious
	bool IsTargetDeadOrUnconscious(AActor* Target) const;
    
	// Calculate revival HP amount
	float CalculateRevivalHP(AActor* Target) const;
	
};
