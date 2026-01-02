// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_Embrace.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Attributes/DBAttr_Primary.h"

UDBGA_Embrace::UDBGA_Embrace()
{
    MPCost = 40.f;
    TPCost = 0.f;
    Range = 800.0f;
    ActionLockSeconds = 3.f;  // 3 second cast time
    DurationSeconds = 0.f;    // Instant effect
    CooldownSeconds = 120.f;  // 2 minute cooldown
    Range = 800.f;
    
    ReviveHPPercentage = 0.25f;      // Revive with 25% HP
    DeathPreventionDuration = 10.f;  // 10 seconds of death prevention
}

void UDBGA_Embrace::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!PayCostsAndLock(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    AActor* CasterActor = GetAvatarActorFromActorInfo();
    
    if (!ASC || !CasterActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Get target from ability data or use self as default
    AActor* TargetActor = nullptr;
    if (TriggerEventData && TriggerEventData->Target.Get())
    {
        TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
    }

    if (!TargetActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    if (!TargetASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    bool bIsTargetDead = IsTargetDeadOrUnconscious(TargetActor);
    
    if (bIsTargetDead)
    {
        // Target is dead - revive them
        if (GE_Revival)
        {
            float RevivalHP = CalculateRevivalHP(TargetActor);
            if (RevivalHP > 0.f)
            {
                FGameplayEffectSpecHandle RevivalSpec = ASC->MakeOutgoingSpec(GE_Revival, GetAbilityLevel(), ASC->MakeEffectContext());
                if (RevivalSpec.IsValid())
                {
                    RevivalSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.HP"), RevivalHP);
                    ASC->ApplyGameplayEffectSpecToTarget(*RevivalSpec.Data.Get(), TargetASC);
                    
                    UE_LOG(LogTemp, Warning, TEXT("Life's Embrace: Revived %s with %f HP"), *TargetActor->GetName(), RevivalHP);
                }
            }
        }
    }
    else
    {
        // Target is alive - apply death prevention
        if (GE_DeathPrevention)
        {
            FGameplayEffectSpecHandle PreventionSpec = ASC->MakeOutgoingSpec(GE_DeathPrevention, GetAbilityLevel(), ASC->MakeEffectContext());
            if (PreventionSpec.IsValid())
            {
                PreventionSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), DeathPreventionDuration);
                ASC->ApplyGameplayEffectSpecToTarget(*PreventionSpec.Data.Get(), TargetASC);
                
                UE_LOG(LogTemp, Warning, TEXT("Life's Embrace: Applied death prevention to %s for %f seconds"), 
                       *TargetActor->GetName(), DeathPreventionDuration);
            }
        }
    }

    // Use base class method for cooldown and cleanup
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UDBGA_Embrace::IsTargetDeadOrUnconscious(AActor* Target) const
{
    if (!Target)
    {
        return false;
    }

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    if (!TargetASC)
    {
        return false;
    }

    const UDBAttr_Primary* TargetStats = TargetASC->GetSet<UDBAttr_Primary>();
    if (!TargetStats)
    {
        return false;
    }

    // Consider dead if HP is 0 or below
    return TargetStats->GetHealth() <= 0.f;
}

float UDBGA_Embrace::CalculateRevivalHP(AActor* Target) const
{
    if (!Target)
    {
        return 0.f;
    }

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    if (!TargetASC)
    {
        return 0.f;
    }

    const UDBAttr_Primary* TargetStats = TargetASC->GetSet<UDBAttr_Primary>();
    if (!TargetStats)
    {
        return 0.f;
    }

    // Revive with 25% of max HP
    return TargetStats->GetMaxHealth() * ReviveHPPercentage;
}