#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_CombatReadiness.generated.h"

/**
 * Evaluator: Checks if enemy is ready to attack
 */
UCLASS()
class DAWNBLADE_API USTEvaluator_CombatReadiness : public USTEvaluator_EnemyBase
{
	GENERATED_BODY()

public:
	USTEvaluator_CombatReadiness();

protected:
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	// ========================================
	// OUTPUTS
	// ========================================

	/** Can the enemy attack right now */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bCanAttack = false;

	/** Is the enemy currently attacking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bIsAttacking = false;

	/** ✅ NEW: Is the enemy's weapon currently drawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bIsArmed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bWeaponDrawn = false;

	/** ✅ NEW: Is the enemy in combat mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bInCombat = false;

	/** Time until next attack is available */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float TimeUntilNextAttack = 0.0f;

	/** Has enough resources (mana/stamina) to attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasResources = true;
};