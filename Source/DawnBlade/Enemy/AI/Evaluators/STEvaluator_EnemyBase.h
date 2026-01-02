// STEvaluator_EnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "STEvaluator_EnemyBase.generated.h"

/**
 * Base evaluator for enemy AI
 * Provides common functionality for all enemy evaluators
 */
UCLASS(Abstract)
class DAWNBLADE_API USTEvaluator_EnemyBase : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

protected:
	/** Get the AI Controller from context */
	class ADBAIController_Enemy* GetAIController(FStateTreeExecutionContext& Context) const;

	/** Get the enemy character from context */
	class ADBEnemyCharacter* GetEnemyCharacter(FStateTreeExecutionContext& Context) const;
};
