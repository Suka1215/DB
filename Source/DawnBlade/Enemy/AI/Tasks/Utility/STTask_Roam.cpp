// STTask_Roam.cpp
#include "STTask_Roam.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "Enemy/DBEnemyCharacter.h"

EStateTreeRunStatus USTTask_Roam::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	AAIController* ControllerPtr = Controller.Get();
    
	if (!ControllerPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Roam: No controller bound!"));
		return EStateTreeRunStatus::Failed;
	}

	ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Roam: No enemy pawn!"));
		return EStateTreeRunStatus::Failed;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Context.GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Roam: No navigation system!"));
		return EStateTreeRunStatus::Failed;
	}

	FVector Origin = Enemy->GetActorLocation();
	FNavLocation ResultLocation;

	if (NavSys->GetRandomPointInNavigableRadius(Origin, RoamRadius, ResultLocation))
	{
		ControllerPtr->MoveToLocation(ResultLocation.Location);
		UE_LOG(LogTemp, Log, TEXT("🚶 %s roaming to location (Distance: %.1f)"), 
			*Enemy->GetName(), FVector::Dist(Origin, ResultLocation.Location));
		return EStateTreeRunStatus::Succeeded;
	}

	UE_LOG(LogTemp, Warning, TEXT("⚠️ Roam: Could not find random location"));
	return EStateTreeRunStatus::Failed;
}