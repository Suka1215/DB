// STEvaluator_ThreatAssessment.cpp
#include "STEvaluator_ThreatAssessment.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"

USTEvaluator_ThreatAssessment::USTEvaluator_ThreatAssessment()
{
	RetreatHealthThreshold = 25.0f;
	CallForHelpThreshold = 50.0f;
}

void USTEvaluator_ThreatAssessment::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	ADBEnemyCharacter* Enemy = GetEnemyCharacter(Context);
    
	if (!Enemy)
	{
		ThreatLevel = 0.0f;
		bShouldRetreat = false;
		bShouldCallForHelp = false;
		HealthPercent = 100.0f;
		return;
	}

	// Get health from GAS
	UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
	if (ASC)
	{
		const UDBAttr_Primary* CoreAttribs = ASC->GetSet<UDBAttr_Primary>();
		if (CoreAttribs)
		{
			float CurrentHealth = CoreAttribs->GetHealth();
			float MaxHealth = CoreAttribs->GetMaxHealth();
            
			if (MaxHealth > 0)
			{
				HealthPercent = (CurrentHealth / MaxHealth) * 100.0f;
			}
		}
	}

	// Calculate threat level based on health
	ThreatLevel = 100.0f - HealthPercent;

	// Determine actions based on health
	bShouldRetreat = HealthPercent <= RetreatHealthThreshold;
	bShouldCallForHelp = HealthPercent <= CallForHelpThreshold;

	UE_LOG(LogTemp, VeryVerbose, TEXT("Threat: %.1f | HP: %.1f%% | Retreat: %s | Help: %s"),
		ThreatLevel,
		HealthPercent,
		bShouldRetreat ? TEXT("YES") : TEXT("NO"),
		bShouldCallForHelp ? TEXT("YES") : TEXT("NO")
	);
}