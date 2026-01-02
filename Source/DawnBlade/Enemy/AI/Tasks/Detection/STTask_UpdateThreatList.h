// STTask_UpdateThreatList.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_UpdateThreatList.generated.h"

class AAIController;

UCLASS()
class DAWNBLADE_API USTTask_UpdateThreatList : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	// ✅ BINDABLE INPUTS
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> Controller;
};