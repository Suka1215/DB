// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckJobAbilities.generated.h"

/**
 * Check and use job-specific abilities based on enemy's job
 * Example: Warrior uses Provoke, White Mage uses Cure
 */
UCLASS()
class DAWNBLADE_API USTTask_CheckJobAbilities : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	/** Enemy to check abilities for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** Job tag to filter abilities (e.g., "Job.Warrior") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGameplayTag JobTag;

	/** Output: Ability that can be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FGameplayTag AbilityToUse;

	/** Output: Can use ability? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bCanUseAbility = false;
};