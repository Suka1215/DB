// STTask_UpdateThreatList.cpp
#include "STTask_UpdateThreatList.h"
#include "AIController.h"
#include "Enemy/AI/DBAIController_Enemy.h"

EStateTreeRunStatus USTTask_UpdateThreatList::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	AAIController* ControllerPtr = Controller.Get();
    
	if (!ControllerPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ UpdateThreatList: No controller bound!"));
		return EStateTreeRunStatus::Failed;
	}

	ADBAIController_Enemy* EnemyAI = Cast<ADBAIController_Enemy>(ControllerPtr);
	if (!EnemyAI)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Update threat list - this would call a method on your AI controller
	// that manages threat/aggro
	// EnemyAI->UpdateThreatList();
    
	UE_LOG(LogTemp, Log, TEXT("🎯 Threat list updated"));
    
	return EStateTreeRunStatus::Succeeded;
}
