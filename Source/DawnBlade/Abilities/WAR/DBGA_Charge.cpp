#include "Abilities/WAR/DBGA_Charge.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

UDBGA_Charge::UDBGA_Charge()
{
    TPCost = 400.f;
    ActionLockSeconds = 0.3f;
    DurationSeconds   = 0.f;     // instant
    CooldownSeconds   = 15.f;    // tweak as desired
    // GE_Cooldown     = GE_Cooldown_Generic (assign in CDO)
}

void UDBGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData*)
{
    if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }
    ACharacter* C = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr;
    if (!C) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

    C->LaunchCharacter(C->GetActorForwardVector()*DashStrength, true, true);
    const FVector Start = C->GetActorLocation() + C->GetActorForwardVector()*50.f;
    const FVector End   = Start + C->GetActorForwardVector()*HitRange;

    TArray<FHitResult> Hits;
    if (C->GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(60.f)))
    {
        if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
        for (auto& H : Hits)
        {
            AActor* T = H.GetActor(); if (!T || T==C) continue;
            if (UAbilitySystemComponent* TASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(T))
            {
                if (GE_PhysDamage)
                {
                    FGameplayEffectSpecHandle D = ASC->MakeOutgoingSpec(GE_PhysDamage, 1.f, ASC->MakeEffectContext());
                    D.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), Power);
                    ASC->ApplyGameplayEffectSpecToTarget(*D.Data.Get(), TASC);
                }
                if (GE_Stagger)
                {
                    FGameplayEffectSpecHandle St = ASC->MakeOutgoingSpec(GE_Stagger, 1.f, ASC->MakeEffectContext());
                    St.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), StaggerSeconds);
                    ASC->ApplyGameplayEffectSpecToTarget(*St.Data.Get(), TASC);
                }
                break; // first hit only
            }
        }
    }
    EndAbility(Handle, Info, ActivationInfo, true, false);
}
