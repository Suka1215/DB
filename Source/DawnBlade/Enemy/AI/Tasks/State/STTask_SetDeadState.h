#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetDeadState.generated.h"

/**
 * Set AI to Dead state
 * - Sets State.AI.Dead tag
 * - Removes all other AI/Combat tags
 */
UCLASS()
class DAWNBLADE_API USTTask_SetDeadState : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;
};