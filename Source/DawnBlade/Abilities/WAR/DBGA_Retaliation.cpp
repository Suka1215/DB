#include "Abilities/WAR/DBGA_Retaliation.h"
#include "AbilitySystemComponent.h"

UDBGA_Retaliation::UDBGA_Retaliation()
{
	ActionLockSeconds = 0.2f;
	DurationSeconds   = 20.f;
	CooldownSeconds   = 180.f;
	// GE_ActiveWindow = GE_ActiveWindow_Generic (optional)
	// GE_Cooldown     = GE_Cooldown_Generic
}

void UDBGA_Retaliation::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData*)
{
	if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (GE_RetaliationBuff)
		{
			FGameplayEffectSpecHandle B = ASC->MakeOutgoingSpec(GE_RetaliationBuff, 1.f, ASC->MakeEffectContext());
			B.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), DurationSeconds);
			ASC->ApplyGameplayEffectSpecToSelf(*B.Data.Get());
		}
	}
	EndAbility(Handle, Info, ActivationInfo, true, false);
}
