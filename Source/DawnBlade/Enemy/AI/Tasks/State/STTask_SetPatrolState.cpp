// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_SetPatrolState.h"

#include "StateTreeExecutionContext.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"

EStateTreeRunStatus USTTask_SetPatrolState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy) return EStateTreeRunStatus::Failed;

	// ✅ Set the tag on the character (for reference/debugging)
	if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
	{
		TagState->SetAI_Patrolling();
		TagState->RemoveTagsInCategory(FName("State.Combat.Engaged"));
        
		UE_LOG(LogTemp, Log, TEXT("🚶 %s → PATROLLING"), *Enemy->GetName());
	}

	// ✅ CRITICAL: Send the event to StateTree to trigger the transition
	FGameplayTag PatrolEventTag = FGameplayTag::RequestGameplayTag(FName("State.AI.Patrolling"));
	Context.SendEvent(PatrolEventTag);
    
	UE_LOG(LogTemp, Warning, TEXT("📡 Sent State.AI.Patrolling EVENT to StateTree"));

	return EStateTreeRunStatus::Succeeded;
}