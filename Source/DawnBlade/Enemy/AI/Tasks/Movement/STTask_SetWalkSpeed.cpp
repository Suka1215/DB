// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_SetWalkSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EStateTreeRunStatus USTTask_SetWalkSpeed::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy) return EStateTreeRunStatus::Failed;

	if (UCharacterMovementComponent* Movement = Enemy->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeed;
        
		UE_LOG(LogTemp, Log, TEXT("🏃 %s speed set to %.0f"), *Enemy->GetName(), WalkSpeed);
	}

	return EStateTreeRunStatus::Succeeded;
}