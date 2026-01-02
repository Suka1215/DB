// STTask_Wait.cpp
#include "STTask_Wait.h"

EStateTreeRunStatus USTTask_Wait::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	TimeElapsed = 0.0f;
    
	// ✅ LOG WHEN WE ENTER THIS TASK
	UE_LOG(LogTemp, Warning, TEXT("⏰ Wait Task Started (Duration: %.1fs)"), Duration);
    
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_Wait::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	TimeElapsed += DeltaTime;

	if (TimeElapsed >= Duration)
	{
		UE_LOG(LogTemp, Log, TEXT("⏰ Wait Task Completed (%.1fs)"), Duration);
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}