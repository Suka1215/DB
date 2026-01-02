// STTask_Rest.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_Rest.generated.h"

class AAIController;

UCLASS()
class DAWNBLADE_API USTTask_Rest : public UStateTreeTaskBlueprintBase
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
	float RestDuration = 3.0f;
};