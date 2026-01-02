// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_SetDeadState.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"

EStateTreeRunStatus USTTask_SetDeadState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy) return EStateTreeRunStatus::Failed;

	if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
	{
		TagState->SetAI_Dead();
		TagState->RemoveTagsInCategory(FName("State.Combat.Engaged"));
        
		UE_LOG(LogTemp, Warning, TEXT("💀 %s → DEAD"), *Enemy->GetName());
	}

	return EStateTreeRunStatus::Succeeded;
}
