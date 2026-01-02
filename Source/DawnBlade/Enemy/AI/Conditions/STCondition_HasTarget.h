// STCondition_HasTarget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STCondition_HasTarget.generated.h"

// ✅ FORWARD DECLARATION
class AAIController;

UCLASS()
class DAWNBLADE_API USTCondition_HasTarget : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> Controller;
};