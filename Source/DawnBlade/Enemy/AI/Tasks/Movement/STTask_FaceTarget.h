// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_FaceTarget.generated.h"

/**
 * Rotate to face the target actor
 * Completes when facing target or timeout
 */
UCLASS()
class DAWNBLADE_API USTTask_FaceTarget : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Target;

	/** Rotation speed (degrees per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float RotationSpeed = 360.0f;

	/** Acceptable angle difference (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float AcceptableAngle = 10.0f;

	/** Max time to try rotating */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float Timeout = 2.0f;

private:
	UPROPERTY()
	float TimeElapsed = 0.0f;
};