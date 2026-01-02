// STTask_LookAround.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_LookAround.generated.h"

class AAIController;

UCLASS()
class DAWNBLADE_API USTTask_LookAround : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	// ✅ BINDABLE INPUTS
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> Controller;
    
	// ✅ PARAMETERS
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float LookDuration = 2.0f;
};