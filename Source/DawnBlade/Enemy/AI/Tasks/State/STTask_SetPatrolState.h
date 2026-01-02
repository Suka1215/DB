#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetPatrolState.generated.h"

/**
 * Set AI to Patrolling state
 * - Sets State.AI.Patrolling tag
 * - Clears combat tags
 */
UCLASS()
class DAWNBLADE_API USTTask_SetPatrolState : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;
};