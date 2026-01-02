#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckTPThreshold.generated.h"

/**
 * Check if enemy has enough TP to use weapon skill
 * TP = Tactical Points (builds from auto-attacks)
 */
UCLASS()
class DAWNBLADE_API USTTask_CheckTPThreshold : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** Minimum TP required (1000 = 1 weapon skill) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float TPThreshold = 1000.0f;

	/** Output: Has enough TP? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasEnoughTP = false;

	/** Output: Current TP amount */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float CurrentTP = 0.0f;
};