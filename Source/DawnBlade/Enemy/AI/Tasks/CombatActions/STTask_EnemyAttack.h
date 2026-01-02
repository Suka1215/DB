// STTask_EnemyAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_EnemyAttack.generated.h"

/**
 * StateTree Task: Enemy performs attack
 * Gets target from AI Controller
 */
UCLASS()
class DAWNBLADE_API USTTask_EnemyAttack : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
	/** Has attack started */
	bool bAttackStarted = false;

	/** Cached references */
	UPROPERTY()
	TObjectPtr<class ADBEnemyCharacter> CachedEnemy;

	UPROPERTY()
	TObjectPtr<AActor> CachedTarget;
};