#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "STTtask_MoveToLocation.generated.h"

UCLASS()
class DAWNBLADE_API USTTask_MoveToLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	/** Enemy performing the move */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	TObjectPtr<class ADBEnemyCharacter> Enemy;

	/** Location to move to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector TargetLocation;

	/** Acceptable radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float AcceptableRadius = 100.0f;

private:
	FAIRequestID MoveRequestID;
};