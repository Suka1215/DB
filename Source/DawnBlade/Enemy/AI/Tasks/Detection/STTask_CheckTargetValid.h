// STTask_CheckTargetValid.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckTargetValid.generated.h"

/**
 * State Tree task that validates if the current target is still valid
 * Checks if target exists, is alive, is within range, etc.
 */
UCLASS()
class DAWNBLADE_API USTTask_CheckTargetValid : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	USTTask_CheckTargetValid(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Enemy;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Target;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float MaxValidDistance = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bCheckAlive = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bCheckPlayerControlled = true;
};