#include "Abilities/WAR/DBGA_Draw.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

UDBGA_Draw::UDBGA_Draw()
{
    ActionLockSeconds = 0.15f;
    Range = 1200.0f;
    DurationSeconds   = 0.f;
    CooldownSeconds   = 30.f;
    // GE_Cooldown = GE_Cooldown_Generic
}

void UDBGA_Draw::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData*)
{
    if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }
    ACharacter* C = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr;
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!C || !ASC) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

    // Aim straight ahead: first pawn along line
    const FVector Start = C->GetActorLocation() + C->GetActorForwardVector()*60.f;
    const FVector End   = Start + C->GetActorForwardVector()*Range;
    FHitResult Hit;
    bool bHit = C->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn);

    if (bHit && Hit.GetActor())
    {
        if (UAbilitySystemComponent* TASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Hit.GetActor()))
        {
            if (GE_Provoke)
            {
                FGameplayEffectSpecHandle P = ASC->MakeOutgoingSpec(GE_Provoke, 1.f, ASC->MakeEffectContext());
                P.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), ForcedSeconds);
                ASC->ApplyGameplayEffectSpecToTarget(*P.Data.Get(), TASC);
            }
            if (GE_Enmity && ThreatBonus > 0.f)
            {
                FGameplayEffectSpecHandle E = ASC->MakeOutgoingSpec(GE_Enmity, 1.f, ASC->MakeEffectContext());
                E.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), ThreatBonus);
                ASC->ApplyGameplayEffectSpecToTarget(*E.Data.Get(), TASC);
            }
        }
    }

    EndAbility(Handle, Info, ActivationInfo, true, false);
}
