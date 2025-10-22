// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_DivinePulse.generated.h"

/**
 * Divine Pulse - Sacrifice HP to restore ally's MP
 * Sacrifices 30% of caster's current HP to restore equal MP to target
 */
UCLASS()
class DAWNBLADE_API UDBGA_DivinePulse : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_DivinePulse();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// HP sacrifice percentage (30% of current HP)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Divine Pulse")
	float HPSacrificePercentage = 0.30f;

	// MP restoration rate (1:1 HP to MP conversion)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Divine Pulse")
	float HPToMPConversionRate = 1.0f;

	// Gameplay Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Divine Pulse")
	TSubclassOf<UGameplayEffect> GE_HPSacrifice;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Divine Pulse")
	TSubclassOf<UGameplayEffect> GE_MPRestoration;

private:
	// Calculate MP restoration amount (simple 1:1 conversion)
	float CalculateMPRestoration(float SacrificedHP) const;
};