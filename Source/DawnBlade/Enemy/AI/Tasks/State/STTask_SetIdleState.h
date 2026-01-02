// ========================================
// STTask_SetIdleState.h
// ========================================
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetIdleState.generated.h"

/**
 * Set AI to Idle state
 * - Sets State.AI.Idle tag
 * - Removes combat tags
 * - Sheathes weapon
 */
UCLASS()
class DAWNBLADE_API USTTask_SetIdleState : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	/** Enemy to set state on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;
};