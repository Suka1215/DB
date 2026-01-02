// STTask_Roam.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_Roam.generated.h"

class AAIController;

UCLASS()
class DAWNBLADE_API USTTask_Roam : public UStateTreeTaskBlueprintBase
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
	float RoamRadius = 500.0f;
};