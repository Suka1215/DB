#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetWalkSpeed.generated.h"

/**
 * Change enemy movement speed
 * Use for walk/run/sprint transitions
 */
UCLASS()
class DAWNBLADE_API USTTask_SetWalkSpeed : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class ACharacter> Enemy;

	/** Speed to set (Walk=150, Run=300, Sprint=600) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float WalkSpeed = 300.0f;
};
