#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckManaForSpell.generated.h"

/**
 * Check if enemy has enough MP to cast a spell
 * Used before attempting to cast magic
 */
UCLASS()
class DAWNBLADE_API USTTask_CheckManaForSpell : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	/** Enemy to check mana for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Enemy;

	/** MP cost of spell */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float ManaCost = 50.0f;

	/** Minimum MP to keep in reserve (don't go below this) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float MinReserveMana = 0.0f;

	/** Output: Has enough mana? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bHasEnoughMana = false;

	/** Output: Current mana */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float CurrentMana = 0.0f;

	/** Output: Max mana */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	float MaxMana = 0.0f;
};
