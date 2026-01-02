// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_MoveToRandomLocation.generated.h"

/**
 * Move to a random location within radius
 * Used for patrolling/wandering behavior
 */
UCLASS()
class DAWNBLADE_API USTTask_MoveToRandomLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class AAIController> Controller;

	/** Search radius from current location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float SearchRadius = 1000.0f;

	/** Acceptable distance to destination */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float AcceptanceRadius = 100.0f;

private:
	UPROPERTY()
	FVector TargetLocation;
    
	UPROPERTY()
	bool bMoveStarted = false;
};