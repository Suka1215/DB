// STEvaluator_ThreatAssessment.h
#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_ThreatAssessment.generated.h"

/**
 * Evaluator: Assesses threat level of current situation
 * Helps AI make tactical decisions
 */
UCLASS()
class DAWNBLADE_API USTEvaluator_ThreatAssessment : public USTEvaluator_EnemyBase
{
	GENERATED_BODY()

public:
	USTEvaluator_ThreatAssessment();

protected:
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	// ========================================
	// OUTPUTS
	// ========================================

	/** Current threat level (0-100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float ThreatLevel = 0.0f;

	/** Should enemy retreat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bShouldRetreat = false;

	/** Should enemy call for help */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bShouldCallForHelp = false;

	/** Health percentage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float HealthPercent = 100.0f;

	// ========================================
	// PARAMETERS
	// ========================================

	/** Health threshold to retreat (percent) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float RetreatHealthThreshold = 25.0f;

	/** Health threshold to call for help (percent) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float CallForHelpThreshold = 50.0f;
};