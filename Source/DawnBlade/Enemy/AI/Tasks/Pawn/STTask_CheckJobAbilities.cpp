// STTask_CheckJobAbilities.cpp - FIXED VERSION
// Fixed: Use AbilityTags from spec instead of GetOwnedGameplayTags

#include "STTask_CheckJobAbilities.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"

EStateTreeRunStatus USTTask_CheckJobAbilities::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    bCanUseAbility = false;
    AbilityToUse = FGameplayTag();

    if (!Enemy) return EStateTreeRunStatus::Succeeded;

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            // Get all granted abilities
            TArray<FGameplayAbilitySpec*> ActivatableAbilities;
            ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
                FGameplayTagContainer(JobTag),
                ActivatableAbilities
            );

            // Find first ability that can be activated
            for (FGameplayAbilitySpec* AbilitySpec : ActivatableAbilities)
            {
                if (AbilitySpec && AbilitySpec->Ability)
                {
                    // Check if ability can be activated (simple check without full validation)
                    bool bCanActivate = true;

                    // Check if ability is currently active (don't activate if already running)
                    if (AbilitySpec->IsActive())
                    {
                        bCanActivate = false;
                    }

                    // Check cooldown
                    if (bCanActivate && AbilitySpec->Ability->CheckCooldown(AbilitySpec->Handle, ASC->AbilityActorInfo.Get()))
                    {
                        bCanActivate = false;
                    }

                    // Check cost (mana, stamina, etc.)
                    if (bCanActivate && !AbilitySpec->Ability->CheckCost(AbilitySpec->Handle, ASC->AbilityActorInfo.Get()))
                    {
                        bCanActivate = false;
                    }

                    if (bCanActivate)
                    {
                        // Get ability tags from the spec (these are public)
                        // AbilitySpec->DynamicAbilityTags contains runtime tags
                        // AbilitySpec->Ability->AbilityTags contains the ability's tags
                        
                        // Try to get tags from the ability's CDO
                        if (AbilitySpec->Ability->AbilityTags.Num() > 0)
                        {
                            AbilityToUse = AbilitySpec->Ability->AbilityTags.First();
                            bCanUseAbility = true;

                            UE_LOG(LogTemp, Log, TEXT("💼 Job ability available: %s"), 
                                *AbilityToUse.ToString());
                            break;
                        }
                        // Fallback: use the job tag itself if no specific ability tag
                        else if (JobTag.IsValid())
                        {
                            AbilityToUse = JobTag;
                            bCanUseAbility = true;

                            UE_LOG(LogTemp, Log, TEXT("💼 Job ability available (using job tag): %s"), 
                                *AbilityToUse.ToString());
                            break;
                        }
                    }
                }
            }

            // If no job-specific ability found, log it
            if (!bCanUseAbility && ActivatableAbilities.Num() > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("💼 Job abilities found but all on cooldown or insufficient resources"));
            }
            else if (ActivatableAbilities.Num() == 0)
            {
                UE_LOG(LogTemp, Log, TEXT("💼 No job abilities found for job tag: %s"), *JobTag.ToString());
            }
        }
    }

    return EStateTreeRunStatus::Succeeded;
}