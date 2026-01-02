// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_DivinePulse.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Attributes/DBAttr_Primary.h"

UDBGA_DivinePulse::UDBGA_DivinePulse()
{
    MPCost = 0.f;  // No MP cost - sacrifices HP instead
    TPCost = 0.f;
    Range = 1000.0f;
    ActionLockSeconds = 0.f;  // Instant cast
    DurationSeconds = 0.f;    // Instant effect
    CooldownSeconds = 120.f;  // 2-minute cooldown
    Range = 1000.f;
    
    HPSacrificePercentage = 0.30f;  // 30% of current HP
    HPToMPConversionRate = 1.0f;    // 1:1 HP to MP conversion
    
    // Set cooldown tag for base class (predefined in .ini)
    CooldownTag = FGameplayTag::RequestGameplayTag("Cooldown.LightMage.DivinePulse");
}

void UDBGA_DivinePulse::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
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

    // ---- Target resolve (no IsValid on TObjectPtr<const AActor>) ----
    AActor* TargetActor = CasterActor; // default to self
    if (TriggerEventData)
    {
        const AActor* TargetConst = TriggerEventData->Target.Get(); // may be null
        if (TargetConst)
        {
            TargetActor = const_cast<AActor*>(TargetConst);
        }
    }
    if (!TargetActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    if (!TargetASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // ---- Stats & sacrifice ----
    const UDBAttr_Primary* CasterStats = ASC->GetSet<UDBAttr_Primary>();
    if (!CasterStats)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const float CurrentHP = CasterStats->GetHealth();
    if (CurrentHP <= 0.f)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    float HPToSacrifice = CurrentHP * HPSacrificePercentage;
    if (HPToSacrifice >= CurrentHP)
    {
        HPToSacrifice = CurrentHP - 1.f; // leave 1 HP minimum
    }
    const float MPToRestore = CalculateMPRestoration(HPToSacrifice);

    // Apply HP sacrifice (negative delta)
    if (GE_HPSacrifice)
    {
        FGameplayEffectSpecHandle SacrificeSpec =
            ASC->MakeOutgoingSpec(GE_HPSacrifice, GetAbilityLevel(), ASC->MakeEffectContext());
        if (SacrificeSpec.IsValid())
        {
            SacrificeSpec.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.HP")), -HPToSacrifice);
            ASC->ApplyGameplayEffectSpecToSelf(*SacrificeSpec.Data.Get());
        }
    }

    // Apply MP restoration to target
    if (GE_MPRestoration)
    {
        FGameplayEffectSpecHandle RestorationSpec =
            ASC->MakeOutgoingSpec(GE_MPRestoration, GetAbilityLevel(), ASC->MakeEffectContext());
        if (RestorationSpec.IsValid())
        {
            RestorationSpec.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.MP")), MPToRestore);
            ASC->ApplyGameplayEffectSpecToTarget(*RestorationSpec.Data.Get(), TargetASC);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Divine Pulse: Sacrificed %.1f HP to restore %.1f MP"),
        HPToSacrifice, MPToRestore);

    // Cooldown & cleanup
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

float UDBGA_DivinePulse::CalculateMPRestoration(float SacrificedHP) const
{
    return SacrificedHP * HPToMPConversionRate;
}