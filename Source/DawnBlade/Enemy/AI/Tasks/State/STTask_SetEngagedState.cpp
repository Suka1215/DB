// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_SetEngagedState.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"

EStateTreeRunStatus USTTask_SetEngagedState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy) return EStateTreeRunStatus::Failed;

	if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
	{
		TagState->SetAI_Engaged();
		TagState->SetCombat_Engaged();
        
		UE_LOG(LogTemp, Warning, TEXT("⚔️ %s → ENGAGED"), *Enemy->GetName());
	}

	return EStateTreeRunStatus::Succeeded;
}
