// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "STTask_ManageBuff.generated.h"

/**
 * Apply and maintain buffs on enemy
 * Re-applies buff when it expires
 */
UCLASS()
class DAWNBLADE_API USTTask_ManageBuff : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	/** Enemy to buff */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** Buff ability tag to apply (e.g., "Ability.Buff.Protect") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGameplayTag BuffAbilityTag;

	/** Tag to check if buff is active (e.g., "Effect.Buff.Protect") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGameplayTag BuffActiveTag;

	/** How often to check if buff needs reapplication (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float CheckInterval = 5.0f;

	/** Should this run continuously? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bContinuous = true;

private:
	UPROPERTY()
	float TimeSinceLastCheck = 0.0f;
};
