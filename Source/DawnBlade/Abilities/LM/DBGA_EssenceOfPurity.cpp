// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_EssenceOfPurity.h"
#include "AbilitySystemComponent.h"

UDBGA_EssenceOfPurity::UDBGA_EssenceOfPurity()
{
    // Passive ability - no costs or cooldowns
    MPCost = 0.f;
    TPCost = 0.f;
    ActionLockSeconds = 0.f;
    DurationSeconds = 0.f;
    CooldownSeconds = 0.f;
    
    HealingBoostPercentage = 0.15f;  // +15% healing power
}

void UDBGA_EssenceOfPurity::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnAvatarSet(ActorInfo, Spec);
    
    // Apply the passive effect when the ability is granted
    ApplyPassiveEffect();
}

void UDBGA_EssenceOfPurity::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    // Remove the passive effect when the ability is removed
    RemovePassiveEffect();
    
    Super::OnRemoveAbility(ActorInfo, Spec);
}

void UDBGA_EssenceOfPurity::ApplyPassiveEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !GE_EssenceOfPurity)
    {
        return;
    }
    
    // Create effect spec
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_EssenceOfPurity, GetAbilityLevel(), ASC->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        // Set the healing boost magnitude
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.HealingPower"), HealingBoostPercentage);
        
        // Apply the effect
        PassiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        
        UE_LOG(LogTemp, Warning, TEXT("Essence of Purity: Applied +%f%% healing boost"), HealingBoostPercentage * 100.f);
    }
}

void UDBGA_EssenceOfPurity::RemovePassiveEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !PassiveEffectHandle.IsValid())
    {
        return;
    }
    
    // Remove the passive effect
    ASC->RemoveActiveGameplayEffect(PassiveEffectHandle);
    PassiveEffectHandle.Invalidate();
    
    UE_LOG(LogTemp, Warning, TEXT("Essence of Purity: Removed healing boost"));
}