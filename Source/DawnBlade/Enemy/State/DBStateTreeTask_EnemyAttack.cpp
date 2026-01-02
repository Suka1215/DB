#include "DBStateTreeTask_EnemyAttack.h"

#include "StateTreeAsyncExecutionContext.h"
#include "Enemy/DBEnemyCharacter.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FDBStateTreeEnemyAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
    {
        FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

        if (!InstanceData.Enemy || !InstanceData.Target)
        {
            return EStateTreeRunStatus::Failed;
        }

        // Bind to attack completed delegate (Epic's pattern)
        InstanceData.Enemy->OnAttackCompleted.BindLambda(
            [WeakContext = Context.MakeWeakExecutionContext()]()
            {
                WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
            }
        );

        // Face target
        FVector Direction = InstanceData.Target->GetActorLocation() - InstanceData.Enemy->GetActorLocation();
        Direction.Z = 0.0f;
        if (!Direction.IsNearlyZero())
        {
            InstanceData.Enemy->SetActorRotation(Direction.Rotation());
        }

        // Perform attack
        InstanceData.Enemy->PerformAutoAttack();
    }

    return EStateTreeRunStatus::Running;
}

void FDBStateTreeEnemyAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
    {
        FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

        if (InstanceData.Enemy)
        {
            // Unbind delegate
            InstanceData.Enemy->OnAttackCompleted.Unbind();
        }
    }
}

#if WITH_EDITOR
FText FDBStateTreeEnemyAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
    return FText::FromString("<b>Enemy Attack</b>");
}
#endif
