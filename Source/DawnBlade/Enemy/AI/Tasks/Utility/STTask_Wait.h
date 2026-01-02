// STTask_Wait.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_Wait.generated.h"

/**
 * StateTree Task: Wait for duration
 */
UCLASS()
class DAWNBLADE_API USTTask_Wait : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	/** Duration to wait */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float Duration = 1.0f;

	/** Time elapsed */
	UPROPERTY()
	float TimeElapsed = 0.0f;
};