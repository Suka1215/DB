// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckHasWeapon.generated.h"

/**
 * Check if enemy has a weapon equipped
 * Outputs boolean result
 */
UCLASS()
class DAWNBLADE_API USTTask_CheckHasWeapon : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** Output: Does enemy have weapon? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasWeapon = false;
};