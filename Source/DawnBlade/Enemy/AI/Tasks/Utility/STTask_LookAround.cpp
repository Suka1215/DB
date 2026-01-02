// STTask_LookAround.cpp
#include "STTask_LookAround.h"
#include "AIController.h"
#include "Components/StateTreeComponent.h"
#include "Enemy/DBEnemyCharacter.h"

EStateTreeRunStatus USTTask_LookAround::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	AAIController* ControllerPtr = Controller.Get();
    
	if (!ControllerPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ LookAround: No controller bound!"));
		return EStateTreeRunStatus::Failed;
	}

	ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
	if (!Enemy)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Stop any movement
	ControllerPtr->StopMovement();
    
	// Rotate to a random direction
	float RandomYaw = FMath::FRandRange(-180.0f, 180.0f);
	FRotator NewRotation = Enemy->GetActorRotation();
	NewRotation.Yaw += RandomYaw;
	Enemy->SetActorRotation(NewRotation);
    
	UE_LOG(LogTemp, Log, TEXT("👀 %s looking around (Yaw: %.1f)"), *Enemy->GetName(), RandomYaw);
    
	return EStateTreeRunStatus::Succeeded;
}