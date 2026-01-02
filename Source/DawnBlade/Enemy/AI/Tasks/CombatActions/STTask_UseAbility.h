// STTask_UseAbility.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "STTask_UseAbility.generated.h"

class AAIController;
class AActor;

UCLASS()
class DAWNBLADE_API USTTask_UseAbility : public UStateTreeTaskBlueprintBase
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
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AbilityRange = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float UseDelay = 0.5f;

private:
	bool bIsUsingAbility = false;
	float UseTimer = 0.0f;
};