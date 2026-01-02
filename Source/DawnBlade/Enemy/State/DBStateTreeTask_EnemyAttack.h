#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "DBStateTreeTask_EnemyAttack.generated.h"

/**
 * Instance data for Enemy Attack Task
 */
USTRUCT()
struct FDBStateTreeEnemyAttackInstanceData
{
	GENERATED_BODY()

	/** Enemy character performing the attack */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<class ADBEnemyCharacter> Enemy;

	/** Target to attack */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Target;
};

/**
 * StateTree Task: Enemy performs attack
 * Based on Epic's FStateTreeComboAttackTask pattern
 */
USTRUCT(meta=(DisplayName="Enemy Attack", Category="Combat"))
struct FDBStateTreeEnemyAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FDBStateTreeEnemyAttackInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** Runs when state is exited */
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};