// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_LinkWithAllies.generated.h"

/**
 * Call for help - alert nearby allies to engage target
 * FFXI-style linking behavior
 */
UCLASS()
class DAWNBLADE_API USTTask_LinkWithAllies : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Target;

	/** Link radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float LinkRadius = 1500.0f;

	/** Max allies to link */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	int32 MaxAllies = 3;
};