// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_MoveToRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"

EStateTreeRunStatus USTTask_MoveToRandomLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Controller || !Controller->GetPawn()) return EStateTreeRunStatus::Failed;

	bMoveStarted = false;

	// Find random reachable point
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Controller->GetWorld());
	if (!NavSys) return EStateTreeRunStatus::Failed;

	FNavLocation ResultLocation;
	bool bFound = NavSys->GetRandomReachablePointInRadius(
		Controller->GetPawn()->GetActorLocation(),
		SearchRadius,
		ResultLocation
	);

	if (!bFound) return EStateTreeRunStatus::Failed;

	TargetLocation = ResultLocation.Location;

	// Start movement
	Controller->MoveToLocation(TargetLocation, AcceptanceRadius);
	bMoveStarted = true;

	UE_LOG(LogTemp, Log, TEXT("🎯 Moving to random location"));

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_MoveToRandomLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (!Controller || !bMoveStarted) return EStateTreeRunStatus::Failed;

	// Check if reached destination
	float Distance = FVector::Dist(Controller->GetPawn()->GetActorLocation(), TargetLocation);
	if (Distance <= AcceptanceRadius)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Reached random location"));
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void USTTask_MoveToRandomLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Controller)
	{
		Controller->StopMovement();
	}
}