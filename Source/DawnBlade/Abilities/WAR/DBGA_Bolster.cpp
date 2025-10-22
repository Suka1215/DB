#include "Abilities/WAR/DBGA_Bolster.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UDBGA_Bolster::UDBGA_Bolster()
{
	ActionLockSeconds = 0.2f;
	DurationSeconds   = 30.f;
	CooldownSeconds   = 180.f;
	// GE_ActiveWindow = GE_ActiveWindow_Generic (optional, if you want a self active tag too)
	// GE_Cooldown     = GE_Cooldown_Generic
}

void UDBGA_Bolster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData*)
{
	if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }
	AActor* Me = Info && Info->AvatarActor.IsValid() ? Info->AvatarActor.Get() : nullptr;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!Me || !ASC || !GE_Bolster) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(Me->GetWorld(), ACharacter::StaticClass(), Characters);
	for (AActor* A : Characters)
	{
		if (FVector::DistSquared(Me->GetActorLocation(), A->GetActorLocation()) > Radius*Radius) continue;
		if (UAbilitySystemComponent* AllyASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(A))
		{
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GE_Bolster, 1.f, ASC->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), DurationSeconds);
			ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), AllyASC);
		}
	}
	EndAbility(Handle, Info, ActivationInfo, true, false);
}
