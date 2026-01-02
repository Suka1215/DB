// STEvaluator_TargetDistance.h
#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_TargetDistance.generated.h"

/**
 * Evaluator: Continuously updates distance to target
 * Runs every tick to keep distance data fresh
 */
UCLASS()
class DAWNBLADE_API USTEvaluator_TargetDistance : public USTEvaluator_EnemyBase
{
	GENERATED_BODY()

public:
	USTEvaluator_TargetDistance();

protected:
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	// ========================================
	// OUTPUTS (Updated every tick)
	// ========================================

	/** Distance to current target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float DistanceToTarget = 0.0f;

	/** Is target within attack range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bInAttackRange = false;

	/** Is target within aggro range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bInAggroRange = false;

	/** Is target in line of sight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasLineOfSight = false;

	// ========================================
	// PARAMETERS
	// ========================================

	/** Attack range threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float AttackRange = 300.0f;

	/** Aggro range threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float AggroRange = 1000.0f;

	/** Should perform line of sight checks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bCheckLineOfSight = true;
};