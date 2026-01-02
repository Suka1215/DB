#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_TargetValidity.generated.h"

UCLASS()
class DAWNBLADE_API USTEvaluator_TargetValidity : public USTEvaluator_EnemyBase
{
	GENERATED_BODY()

public:
	USTEvaluator_TargetValidity();

protected:
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	// ========================================
	// OUTPUTS
	// ========================================

	/** The current valid target actor (or nullptr if no valid target) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	TObjectPtr<AActor> ValidTarget = nullptr;

	/** Is current target valid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasValidTarget = false;

	/** Is target alive */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bTargetIsAlive = false;

	// ========================================
	// PARAMETERS
	// ========================================

	/** Auto-clear invalid targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bAutoClearInvalidTargets = true;
};