// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_StopMovement.h"
#include "AIController.h"

EStateTreeRunStatus USTTask_StopMovement::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Controller) return EStateTreeRunStatus::Failed;

	Controller->StopMovement();
    
	UE_LOG(LogTemp, Log, TEXT("🛑 Movement stopped"));

	return EStateTreeRunStatus::Succeeded;
}