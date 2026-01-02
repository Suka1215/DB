#include "STEvaluator_TargetValidity.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "GameFramework/Character.h"

USTEvaluator_TargetValidity::USTEvaluator_TargetValidity()
{
	bAutoClearInvalidTargets = true;
}

void USTEvaluator_TargetValidity::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	ADBAIController_Enemy* AIController = GetAIController(Context);
    
	if (!AIController)
	{
		ValidTarget = nullptr;
		bHasValidTarget = false;
		bTargetIsAlive = false;
		return;
	}

	AActor* Target = AIController->GetTargetActor();

	// Check if target exists and is valid
	if (!Target || !IsValid(Target))
	{
		ValidTarget = nullptr;
		bHasValidTarget = false;
		bTargetIsAlive = false;

		if (bAutoClearInvalidTargets)
		{
			AIController->ClearTarget();
		}
		return;
	}

	// Check if target is alive (for characters)
	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (TargetCharacter)
	{
		bTargetIsAlive = !TargetCharacter->GetMesh()->IsSimulatingPhysics(); // Simple death check
	}
	else
	{
		bTargetIsAlive = true; // Non-characters are always "alive"
	}

	// Target is valid if it exists and is alive
	bHasValidTarget = bTargetIsAlive;

	// ✅ Set the valid target output
	if (bHasValidTarget)
	{
		ValidTarget = Target;
	}
	else
	{
		ValidTarget = nullptr;
	}

	// Auto-clear dead targets
	if (!bTargetIsAlive && bAutoClearInvalidTargets)
	{
		UE_LOG(LogTemp, Log, TEXT("🪦 Target died, clearing"));
		AIController->ClearTarget();
		ValidTarget = nullptr;
		bHasValidTarget = false;
	}
}