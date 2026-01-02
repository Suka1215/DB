// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetEngagedState.generated.h"

/**
 * Set AI to Engaged (combat) state
 * - Sets State.AI.Engaged tag
 * - Sets State.Combat.Engaged tag
 */
UCLASS()
class DAWNBLADE_API USTTask_SetEngagedState : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;
};