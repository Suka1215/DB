#include "STTask_SetIdleState.h"

#include "StateTreeExecutionContext.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"

EStateTreeRunStatus USTTask_SetIdleState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy) return EStateTreeRunStatus::Failed;

	if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
	{
		TagState->SetAI_Idle();
		TagState->RemoveTagsInCategory(FName("State.Combat.Engaged"));
        
		UE_LOG(LogTemp, Log, TEXT("✅ %s → IDLE"), *Enemy->GetName());
	}

	// ✅ CRITICAL: Send the event to StateTree to trigger the transition
	FGameplayTag PatrolEventTag = FGameplayTag::RequestGameplayTag(FName("State.AI.Idle"));
	Context.SendEvent(PatrolEventTag);
    
	UE_LOG(LogTemp, Warning, TEXT("📡 Sent State.AI.Idle EVENT to StateTree"));

	return EStateTreeRunStatus::Succeeded;
}
