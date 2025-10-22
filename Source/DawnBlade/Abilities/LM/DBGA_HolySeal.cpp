// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_HolySeal.h"
#include "AbilitySystemComponent.h"

UDBGA_HolySeal::UDBGA_HolySeal()
{
    MPCost = 15.f;
    TPCost = 0.f;
    ActionLockSeconds = 0.f;  // Instant cast
    DurationSeconds = 0.f;    // Instant application, buff has its own duration
    CooldownSeconds = 60.f;   // 1 minute cooldown
    
    SealDuration = 10.f;       // 10 seconds to use the enhanced heal
    HealingMultiplier = 2.0f;  // Double healing power
}

void UDBGA_HolySeal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!PayCostsAndLock(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !GE_HolySeal)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Apply Holy Seal effect to self
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_HolySeal, GetAbilityLevel(), ASC->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        // Set duration and healing multiplier
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), SealDuration);
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.HealingMultiplier"), HealingMultiplier);
        
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        
        UE_LOG(LogTemp, Warning, TEXT("Holy Seal: Applied %fx healing multiplier for %f seconds"), 
               HealingMultiplier, SealDuration);
    }

    // Use base class method for cooldown and cleanup
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}