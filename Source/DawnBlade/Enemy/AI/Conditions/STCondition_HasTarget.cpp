// STCondition_HasTarget.cpp
#include "STCondition_HasTarget.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AIController.h"

bool USTCondition_HasTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
	// ✅ GET RAW POINTER FROM TObjectPtr
	AAIController* ControllerPtr = Controller.Get();
	if (!ControllerPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ HasTarget: No controller bound!"));
		return false;
	}

	ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
	if (!Enemy)
	{
		return false;
	}

	AActor* Target = Enemy->GetCurrentTarget();
	bool bHasTarget = Target != nullptr && IsValid(Target);

	UE_LOG(LogTemp, Warning, TEXT("👁️ HasTarget Check: %s (Target: %s)"), 
		bHasTarget ? TEXT("TRUE") : TEXT("FALSE"),
		Target ? *Target->GetName() : TEXT("None"));

	return bHasTarget;
}