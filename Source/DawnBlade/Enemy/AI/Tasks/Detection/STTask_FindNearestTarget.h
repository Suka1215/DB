// STTask_FindNearestTarget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_FindNearestTarget.generated.h"

/**
 * State Tree task that finds the nearest valid target (player) within detection range
 * Used during patrol/idle states to detect and engage players
 */
UCLASS()
class DAWNBLADE_API USTTask_FindNearestTarget : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	USTTask_FindNearestTarget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> OutTarget;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Enemy;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float DetectionRadius = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bRequireLineOfSight = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TEnumAsByte<ECollisionChannel> LineOfSightChannel = ECC_Visibility;
};