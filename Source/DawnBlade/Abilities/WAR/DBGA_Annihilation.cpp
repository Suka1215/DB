#include "Abilities/WAR/DBGA_Annihilation.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Attributes/DBAttr_Combat.h"
#include "GameFramework/Character.h"

UDBGA_Annihilation::UDBGA_Annihilation()
{
    ActionLockSeconds = 0.5f;
    Range = 250.0f;
    DurationSeconds   = 0.f;    // instant
    CooldownSeconds   = 30.f;   // 30s CD
    // GE_Cooldown     = GE_Cooldown_Generic (assign in CDO)
}

void UDBGA_Annihilation::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* Info, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData*)
{
    if (!PayCostsAndLock(Handle, Info, ActivationInfo)) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

    ACharacter* C = Info ? Cast<ACharacter>(Info->AvatarActor.Get()) : nullptr;
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!C || !ASC || !GE_PhysDamage) { EndAbility(Handle, Info, ActivationInfo, true, true); return; }

    // Read & spend all TP
    const float TP = ASC->GetNumericAttribute(UDBAttr_Combat::GetTPAttribute());
    if (GE_TPDelta && TP > 0.f)
    {
        FGameplayEffectSpecHandle TPSpend = ASC->MakeOutgoingSpec(GE_TPDelta, 1.f, ASC->MakeEffectContext());
        TPSpend.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.TP"), -TP);
        ASC->ApplyGameplayEffectSpecToSelf(*TPSpend.Data.Get());
    }

    // Target straight ahead
    const FVector Start = C->GetActorLocation() + C->GetActorForwardVector()*60.f;
    const FVector End   = Start + C->GetActorForwardVector()*Range;
    FHitResult Hit;
    bool bHit = C->GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius));

    if (bHit && Hit.GetActor())
    {
        if (UAbilitySystemComponent* TASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Hit.GetActor()))
        {
            const float PowerScale = 1.f + (TP / FMath::Max(1.f, TPScale));
            FGameplayEffectSpecHandle D = ASC->MakeOutgoingSpec(GE_PhysDamage, 1.f, ASC->MakeEffectContext());
            D.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), BasePower * PowerScale);
            ASC->ApplyGameplayEffectSpecToTarget(*D.Data.Get(), TASC);

            if (GE_Enmity && BonusEnmity > 0.f)
            {
                FGameplayEffectSpecHandle Enm = ASC->MakeOutgoingSpec(GE_Enmity, 1.f, ASC->MakeEffectContext());
                Enm.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Power"), BonusEnmity);
                ASC->ApplyGameplayEffectSpecToTarget(*Enm.Data.Get(), TASC);
            }
        }
    }

    EndAbility(Handle, Info, ActivationInfo, true, false);
}
