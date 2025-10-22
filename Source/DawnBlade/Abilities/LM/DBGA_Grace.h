// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Grace.generated.h"

/**
 * Lightstep Grace - Self-buff allowing movement while casting
 * Instant cast, 20 MP cost, 1 minute cooldown, 15 second duration
 */
UCLASS()
class DAWNBLADE_API UDBGA_Grace : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_Grace();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Duration of the grace effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lightstep Grace")
	float GraceDuration = 15.f;

	// Movement speed bonus while active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lightstep Grace")
	float MovementSpeedBonus = 0.25f;  // +25% movement speed

	// Gameplay Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lightstep Grace")
	TSubclassOf<UGameplayEffect> GE_LightstepGrace;
	
};
