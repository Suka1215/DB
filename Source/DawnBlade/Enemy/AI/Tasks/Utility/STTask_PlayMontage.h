// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_PlayMontage.generated.h"

/**
 * Generic montage player
 * Plays animation and waits for completion
 */
UCLASS()
class DAWNBLADE_API USTTask_PlayMontage : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	// ✅ Add explicit constructor declaration
	USTTask_PlayMontage(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	TObjectPtr<class UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float PlayRate = 1.0f;

private:
	UPROPERTY()
	bool bMontageFinished = false;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted);
};