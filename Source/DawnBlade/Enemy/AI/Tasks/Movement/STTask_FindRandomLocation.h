// STTask_FindRandomLocation.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_FindRandomLocation.generated.h"

/**
 * StateTree Task: Find random patrol location
 */
UCLASS()
class DAWNBLADE_API USTTask_FindRandomLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	/** Enemy character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	TObjectPtr<class ADBEnemyCharacter> Enemy;

	/** Search radius for random point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float SearchRadius = 500.0f;

	/** Output: Random location found */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FVector RandomLocation;
};