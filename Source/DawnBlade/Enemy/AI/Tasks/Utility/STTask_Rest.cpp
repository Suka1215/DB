// STTask_Rest.cpp
#include "STTask_Rest.h"
#include "AIController.h"
#include "Enemy/DBEnemyCharacter.h"

EStateTreeRunStatus USTTask_Rest::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	AAIController* ControllerPtr = Controller.Get();
    
	if (!ControllerPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Rest: No controller bound!"));
		return EStateTreeRunStatus::Failed;
	}

	ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
	if (!Enemy)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Stop any movement
	ControllerPtr->StopMovement();
    
	UE_LOG(LogTemp, Log, TEXT("😴 %s resting for %.1f seconds"), *Enemy->GetName(), RestDuration);
    
	return EStateTreeRunStatus::Succeeded;
}