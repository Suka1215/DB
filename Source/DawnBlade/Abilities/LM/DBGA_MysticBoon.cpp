// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/LM/DBGA_MysticBoon.h"
#include "AbilitySystemComponent.h"
#include "Components/DBJobComponent.h"

UDBGA_MysticBoon::UDBGA_MysticBoon()
{
    // Passive ability - no costs or cooldowns
    MPCost = 0.f;
    TPCost = 0.f;
    ActionLockSeconds = 0.f;
    DurationSeconds = 0.f;
    CooldownSeconds = 0.f;
    
    MysticBoonRank = 1;
    
    // Initialize healing bonus per rank
    HealingBonusPerRank = {0.10f, 0.15f, 0.20f, 0.25f, 0.30f};  // 10%, 15%, 20%, 25%, 30%
    RequiredLevelsPerRank = {5, 15, 25, 35, 45};
}

void UDBGA_MysticBoon::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnAvatarSet(ActorInfo, Spec);
    
    // Apply the passive effect when the ability is granted
    ApplyPassiveEffect();
}

void UDBGA_MysticBoon::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    // Remove the passive effect when the ability is removed
    RemovePassiveEffect();
    
    Super::OnRemoveAbility(ActorInfo, Spec);
}

void UDBGA_MysticBoon::ApplyPassiveEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !GE_MysticBoon)
    {
        return;
    }
    
    // Get character level to determine rank
    int32 CharacterLevel = 1;
    if (const AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        if (const UDBJobComponent* JobComponent = Avatar->FindComponentByClass<UDBJobComponent>())
        {
            CharacterLevel = JobComponent->Level;
        }
    }
    
    // Determine the appropriate rank for this level
    int32 CurrentRank = GetRankForLevel(CharacterLevel);
    
    if (CurrentRank <= 0 || CurrentRank > HealingBonusPerRank.Num())
    {
        return; // Invalid rank or level too low
    }
    
    // Remove existing effect if any
    RemovePassiveEffect();
    
    // Create effect spec
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_MysticBoon, GetAbilityLevel(), ASC->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        // Set the healing boost magnitude based on rank
        float HealingBonus = HealingBonusPerRank[CurrentRank - 1];  // Array is 0-indexed
        SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("SetByCaller.HealingPower"), HealingBonus);
        
        // Add rank-specific tag (without RequestGameplayTag - use predefined tags)
        FString RankTagString = FString::Printf(TEXT("State.MysticBoon.Rank%d"), CurrentRank);
        FGameplayTag RankTag = FGameplayTag::RequestGameplayTag(*RankTagString);
        SpecHandle.Data->DynamicGrantedTags.AddTag(RankTag);
        
        // Apply the effect
        PassiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        
        UE_LOG(LogTemp, Warning, TEXT("Mystic Boon Rank %d: Applied +%f%% healing boost"), 
               CurrentRank, HealingBonus * 100.f);
    }
}

void UDBGA_MysticBoon::RemovePassiveEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !PassiveEffectHandle.IsValid())
    {
        return;
    }
    
    // Remove the passive effect
    ASC->RemoveActiveGameplayEffect(PassiveEffectHandle);
    PassiveEffectHandle.Invalidate();
    
    UE_LOG(LogTemp, Warning, TEXT("Mystic Boon: Removed healing boost"));
}

int32 UDBGA_MysticBoon::GetRankForLevel(int32 CharacterLevel) const
{
    int32 Rank = 0;
    
    // Find the highest rank the character qualifies for
    for (int32 i = 0; i < RequiredLevelsPerRank.Num(); ++i)
    {
        if (CharacterLevel >= RequiredLevelsPerRank[i])
        {
            Rank = i + 1;  // Ranks are 1-indexed
        }
        else
        {
            break;  // Character level too low for this rank
        }
    }
    
    return Rank;
}