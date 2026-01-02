// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_UseWeaponSkill.generated.h"

/**
 * Use a TP-based weapon skill
 * Consumes TP and performs special attack
 */
UCLASS()
class DAWNBLADE_API USTTask_UseWeaponSkill : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class ACharacter> Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<AActor> Target;

	/** Weapon skill ability tag (e.g., "Ability.WeaponSkill.FastBlade") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGameplayTag WeaponSkillTag;

	/** TP cost */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float TPCost = 1000.0f;

private:
	UPROPERTY()
	bool bSkillActivated = false;
};