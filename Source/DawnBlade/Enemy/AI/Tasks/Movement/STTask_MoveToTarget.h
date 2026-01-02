// STTask_MoveToTarget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_MoveToTarget.generated.h"

// Forward declarations
class AAIController;
class AActor;

UCLASS()
class DAWNBLADE_API USTTask_MoveToTarget : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	USTTask_MoveToTarget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	// ✅ This will be bound to Context.CurrentTarget in the StateTree editor
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Target;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0", ClampMax = "2000.0"))
	float AcceptableRadius = 100.0f;
};