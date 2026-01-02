// STTask_CheckTargetValid.cpp
#include "STTask_CheckTargetValid.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

USTTask_CheckTargetValid::USTTask_CheckTargetValid(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus USTTask_CheckTargetValid::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("STTask_CheckTargetValid: Enemy is null"));
        return EStateTreeRunStatus::Failed;
    }

    // Check if target exists
    if (!Target || !IsValid(Target))
    {
        UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target is null or invalid"), 
            *Enemy->GetName());
        return EStateTreeRunStatus::Failed;
    }

    // Check if target is pending kill
    if (Target->IsPendingKillPending())
    {
        UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target is pending kill"), 
            *Enemy->GetName());
        return EStateTreeRunStatus::Failed;
    }

    // Check if target is player controlled (if required)
    if (bCheckPlayerControlled)
    {
        ACharacter* TargetCharacter = Cast<ACharacter>(Target);
        if (!TargetCharacter || !TargetCharacter->IsPlayerControlled())
        {
            UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target is not player controlled"), 
                *Enemy->GetName());
            return EStateTreeRunStatus::Failed;
        }
    }

    // Check if target is alive (if required)
    if (bCheckAlive)
    {
        // Try to check via Ability System Component first
        IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Target);
        if (ASI)
        {
            UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
            if (ASC)
            {
                // Check for dead tag
                FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
                if (ASC->HasMatchingGameplayTag(DeadTag))
                {
                    UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target has Dead tag"), 
                        *Enemy->GetName());
                    return EStateTreeRunStatus::Failed;
                }
            }
        }

        // Fallback: Check if character is alive via health or pawn death
        ACharacter* TargetCharacter = Cast<ACharacter>(Target);
        if (TargetCharacter && TargetCharacter->GetMesh())
        {
            // Check if ragdoll physics is enabled (common indicator of death)
            if (TargetCharacter->GetMesh()->IsSimulatingPhysics())
            {
                UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target is simulating physics (dead)"), 
                    *Enemy->GetName());
                return EStateTreeRunStatus::Failed;
            }
        }
    }

    // Check distance
    float Distance = FVector::Dist(Enemy->GetActorLocation(), Target->GetActorLocation());
    if (Distance > MaxValidDistance)
    {
        UE_LOG(LogTemp, Verbose, TEXT("❌ %s target validation failed: Target too far (%.1f > %.1f)"), 
            *Enemy->GetName(), Distance, MaxValidDistance);
        return EStateTreeRunStatus::Failed;
    }

    // All checks passed
    UE_LOG(LogTemp, Verbose, TEXT("✅ %s target validation passed: %s is valid at distance %.1f"), 
        *Enemy->GetName(), *Target->GetName(), Distance);
    return EStateTreeRunStatus::Succeeded;
}