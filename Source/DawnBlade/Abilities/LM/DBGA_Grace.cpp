// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_Grace.h"
#include "AbilitySystemComponent.h"

UDBGA_Grace::UDBGA_Grace()
{
    MPCost = 20.f;
    TPCost = 0.f;
    ActionLockSeconds = 0.f;  // Instant cast
    DurationSeconds = 0.f;    // Instant application, buff has its own duration
    CooldownSeconds = 60.f;   // 1 minute cooldown
    
    GraceDuration = 15.f;        // 15 seconds of grace
    MovementSpeedBonus = 0.25f;  // +25% movement speed
}

void UDBGA_Grace::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!PayCostsAndLock(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !GE_LightstepGrace)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Apply Lightstep Grace effect to self
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_LightstepGrace, GetAbilityLevel(), ASC->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        // Set duration and movement speed bonus
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.Duration"), GraceDuration);
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.MovementSpeed"), MovementSpeedBonus);
        
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        
        UE_LOG(LogTemp, Warning, TEXT("Lightstep Grace: Applied movement while casting for %f seconds"), GraceDuration);
    }

    // Use base class method for cooldown and cleanup
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}