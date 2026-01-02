// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_UseWeaponSkill.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Attributes/DBAttr_Combat.h"

EStateTreeRunStatus USTTask_UseWeaponSkill::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy || !Target) return EStateTreeRunStatus::Failed;

	bSkillActivated = false;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			// Check TP
			if (const UDBAttr_Combat* CombatAttribs = ASC->GetSet<UDBAttr_Combat>())
			{
				if (CombatAttribs->GetTP() < TPCost)
				{
					UE_LOG(LogTemp, Warning, TEXT("❌ Not enough TP for weapon skill"));
					return EStateTreeRunStatus::Failed;
				}
			}

			// Activate weapon skill ability
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(WeaponSkillTag);

			if (ASC->TryActivateAbilitiesByTag(TagContainer))
			{
				bSkillActivated = true;
				UE_LOG(LogTemp, Warning, TEXT("⚔️💥 %s using weapon skill!"), *Enemy->GetName());
                
				// TP will be consumed by the ability itself via gameplay effect
                
				return EStateTreeRunStatus::Running;
			}
		}
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus USTTask_UseWeaponSkill::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// Wait for ability/montage to complete
	// This is simplified - in practice you'd check if montage is still playing
	if (bSkillActivated)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}