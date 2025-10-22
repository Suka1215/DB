#include "Abilities/Spells/DBGA_Cure.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"
#include "Components/DBJobComponent.h"

UDBGA_Cure::UDBGA_Cure()
{
	// Instead of hardcoding, just point this GA at the row in DT_Abilities
	AbilityRowName = "Cure1"; // Change per rank: Cure1, Cure2, Cure3, Cure4
}

void UDBGA_Cure::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	const FDBAbilityData* Data = GetAbilityData();
	if (!Data || !GE_Cure)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Costs & timings from DataTable
	MPCost            = Data->MPCost;
	TPCost            = Data->TPCost;
	ActionLockSeconds = Data->CastTime;
	CooldownSeconds   = Data->Cooldown;
	Range             = Data->Range;

	if (!PayCostsAndLock(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* Caster = GetAvatarActorFromActorInfo();
	if (!ASC || !Caster) return;

	// Default target = self
	AActor* TargetActor = Caster;
	if (TriggerEventData && TriggerEventData->Target.Get())
	{
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!TargetASC) return;

	// Build Cure GE
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_Cure, Data->Rank, ASC->MakeEffectContext());
	if (SpecHandle.IsValid())
	{
		// Apply potency scaling by job
		const float Affinity = GetJobHealingMultiplier(Caster);
		const float FinalHeal = Data->Potency * Affinity;

		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing")),
			FinalHeal
		);

		ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

		UE_LOG(LogTemp, Log, TEXT("Cure: %s healed %s for %f"),
			*Caster->GetName(),
			*TargetActor->GetName(),
			FinalHeal);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

float UDBGA_Cure::GetJobHealingMultiplier(const AActor* Caster) const
{
	if (const UDBJobComponent* JobComp = Caster->FindComponentByClass<UDBJobComponent>())
	{
		switch (JobComp->CurrentJob)
		{
		case EDBJob::LightMage: return 1.0f;
		case EDBJob::Paladin:   return 0.7f;
		case EDBJob::Warrior:   return 0.4f;
		default:                  return 0.5f;
		}
	}
	return 1.0f;
}
