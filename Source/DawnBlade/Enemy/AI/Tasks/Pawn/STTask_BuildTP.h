// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_BuildTP.generated.h"

/**
 * Build TP (Tactical Points) from auto-attacks
 * TP accumulates with each hit, used for weapon skills
 * FFXI mechanic - 100 TP per hit, 1000 TP = 1 weapon skill
 */
UCLASS()
class DAWNBLADE_API USTTask_BuildTP : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	/** Enemy building TP */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** TP gained per auto-attack hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float TPPerHit = 100.0f;

	/** Should this task run continuously? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bContinuous = true;

private:
	UPROPERTY()
	bool bBound = false;

	UFUNCTION()
	void OnAttackHit();
};