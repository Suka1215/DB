// Copyright DawnBlade. All Rights Reserved.

#include "STTask_SetPursuingState.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"
#include "GameFramework/Character.h"

USTTask_SetPursuingState::USTTask_SetPursuingState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EStateTreeRunStatus USTTask_SetPursuingState::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ STTask_SetPursuingState: No enemy reference"));
		return EStateTreeRunStatus::Failed;
	}

	// Get AI State Component
	UDBAIStateComponent* AIState = Enemy->FindComponentByClass<UDBAIStateComponent>();
	if (!AIState)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ STTask_SetPursuingState: %s has no DBAIStateComponent"), *Enemy->GetName());
		return EStateTreeRunStatus::Failed;
	}

	// Set pursuing state
	AIState->SetAI_Pursuing();

	UE_LOG(LogTemp, Log, TEXT("🏃 %s → State: PURSUING"), *Enemy->GetName());

	return EStateTreeRunStatus::Succeeded;
}