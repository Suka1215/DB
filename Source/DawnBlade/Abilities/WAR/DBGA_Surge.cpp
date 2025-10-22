#include "Abilities/WAR/DBGA_Surge.h"
#include "AbilitySystemComponent.h"

UDBGA_Surge::UDBGA_Surge()
{
	ActionLockSeconds = 0.2f;
	DurationSeconds   = 10.f;
	CooldownSeconds   = 60.f;           // adjust in CDO if you want a different CD
	// GE_ActiveWindow = GE_ActiveWindow_Generic (assign in CDO if you want an "active" tag while buff lasts)
	// GE_Cooldown     = GE_Cooldown_Generic     (assign in CDO)
}

void UDBGA_Surge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData*)
{
	if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (GE_SurgeBuff)
		{
			FGameplayEffectSpecHandle S = ASC->MakeOutgoingSpec(GE_SurgeBuff, 1.f, ASC->MakeEffectContext());
			S.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), DurationSeconds);
			S.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"),   Power);
			ASC->ApplyGameplayEffectSpecToSelf(*S.Data.Get());
		}
	}
	EndAbility(Handle, Info, ActivationInfo, true, false);
}
