// STTask_EngageTarget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_EngageTarget.generated.h"

class AAIController;
class AActor;

UCLASS()
class DAWNBLADE_API USTTask_EngageTarget : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	// ✅ BINDABLE INPUTS
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> Controller;
    
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Target;
    
	// ✅ PARAMETERS
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float EngageDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AcceptanceRadius = 50.0f;

private:
	bool bWeaponDrawn = false;
};