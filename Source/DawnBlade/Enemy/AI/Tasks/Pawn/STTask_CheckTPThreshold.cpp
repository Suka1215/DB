// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_CheckTPThreshold.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Attributes/DBAttr_Combat.h"

EStateTreeRunStatus USTTask_CheckTPThreshold::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	bHasEnoughTP = false;
	CurrentTP = 0.0f;

	if (!Enemy) return EStateTreeRunStatus::Succeeded;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UDBAttr_Combat* CombatAttribs = ASC->GetSet<UDBAttr_Combat>())
			{
				CurrentTP = CombatAttribs->GetTP();
				bHasEnoughTP = CurrentTP >= TPThreshold;

				UE_LOG(LogTemp, Log, TEXT("⚡ TP Check: %.0f / %.0f (%s)"), 
					CurrentTP, 
					TPThreshold,
					bHasEnoughTP ? TEXT("READY") : TEXT("NOT READY"));
			}
		}
	}

	return EStateTreeRunStatus::Succeeded;
}