// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_ManageBuff.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

EStateTreeRunStatus USTTask_ManageBuff::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    TimeSinceLastCheck = 0.0f;

    if (!Enemy) return EStateTreeRunStatus::Failed;

    // Try to apply buff immediately
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            // Check if buff already active
            if (ASC->HasMatchingGameplayTag(BuffActiveTag))
            {
                UE_LOG(LogTemp, Log, TEXT("✨ Buff already active: %s"), *BuffActiveTag.ToString());
                return bContinuous ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
            }

            // Try to activate buff ability
            FGameplayTagContainer TagContainer;
            TagContainer.AddTag(BuffAbilityTag);

            if (ASC->TryActivateAbilitiesByTag(TagContainer))
            {
                UE_LOG(LogTemp, Warning, TEXT("✨ Applied buff: %s"), *BuffAbilityTag.ToString());
            }
        }
    }

    return bContinuous ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus USTTask_ManageBuff::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!Enemy) return EStateTreeRunStatus::Failed;

    TimeSinceLastCheck += DeltaTime;

    if (TimeSinceLastCheck >= CheckInterval)
    {
        TimeSinceLastCheck = 0.0f;

        // Check if buff needs reapplication
        if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
        {
            if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
            {
                // If buff is not active, try to reapply
                if (!ASC->HasMatchingGameplayTag(BuffActiveTag))
                {
                    FGameplayTagContainer TagContainer;
                    TagContainer.AddTag(BuffAbilityTag);

                    if (ASC->TryActivateAbilitiesByTag(TagContainer))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("✨ Reapplied buff: %s"), *BuffAbilityTag.ToString());
                    }
                }
            }
        }
    }

    return EStateTreeRunStatus::Running;
}