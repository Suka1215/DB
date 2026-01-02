// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_CheckManaForSpell.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Attributes/DBAttr_Primary.h"

EStateTreeRunStatus USTTask_CheckManaForSpell::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	bHasEnoughMana = false;
	CurrentMana = 0.0f;
	MaxMana = 0.0f;

	if (!Enemy) return EStateTreeRunStatus::Succeeded;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UDBAttr_Primary* PrimaryAttribs = ASC->GetSet<UDBAttr_Primary>())
			{
				CurrentMana = PrimaryAttribs->GetMana();
				MaxMana = PrimaryAttribs->GetMaxMana();

				float AvailableMana = CurrentMana - MinReserveMana;
				bHasEnoughMana = AvailableMana >= ManaCost;

				if (bHasEnoughMana)
				{
					UE_LOG(LogTemp, Log, TEXT("🔮 Mana check: %.0f MP available (need %.0f) ✅"), 
						CurrentMana, ManaCost);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("🔮 Mana check: %.0f MP available (need %.0f) ❌"), 
						CurrentMana, ManaCost);
				}
			}
		}
	}

	return EStateTreeRunStatus::Succeeded;
}