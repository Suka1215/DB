// STTask_UseAbility.cpp
#include "STTask_UseAbility.h"
#include "AIController.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

EStateTreeRunStatus USTTask_UseAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    AAIController* ControllerPtr = Controller.Get();
    AActor* TargetPtr = Target.Get();
    
    if (!ControllerPtr || !TargetPtr)
    {
        return EStateTreeRunStatus::Failed;
    }

    ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
    if (!Enemy)
    {
        return EStateTreeRunStatus::Failed;
    }

    // Check if target is in range
    float DistanceToTarget = FVector::Dist(Enemy->GetActorLocation(), TargetPtr->GetActorLocation());
    if (DistanceToTarget > AbilityRange)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ %s target out of ability range (Distance: %.1f, Max: %.1f)"), 
            *Enemy->GetName(), DistanceToTarget, AbilityRange);
        return EStateTreeRunStatus::Failed;
    }

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s has no AbilitySystemComponent"), *Enemy->GetName());
        return EStateTreeRunStatus::Failed;
    }

    // Check if we have the ability
    if (!AbilityTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ UseAbility: Invalid ability tag"));
        return EStateTreeRunStatus::Failed;
    }

    bIsUsingAbility = true;
    UseTimer = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("⚡ %s preparing to use %s..."), 
        *Enemy->GetName(), *AbilityTag.ToString());

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_UseAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!bIsUsingAbility)
    {
        return EStateTreeRunStatus::Failed;
    }

    UseTimer += DeltaTime;

    if (UseTimer >= UseDelay)
    {
        AAIController* ControllerPtr = Controller.Get();
        if (!ControllerPtr)
        {
            return EStateTreeRunStatus::Failed;
        }

        ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
        if (!Enemy)
        {
            return EStateTreeRunStatus::Failed;
        }

        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
        if (!ASC)
        {
            return EStateTreeRunStatus::Failed;
        }

        // Try to activate the ability
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(AbilityTag);

        bool bSuccess = ASC->TryActivateAbilitiesByTag(TagContainer);
        
        if (bSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("💥 %s used %s!"), 
                *Enemy->GetName(), *AbilityTag.ToString());
            return EStateTreeRunStatus::Succeeded;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ %s failed to use %s"), 
                *Enemy->GetName(), *AbilityTag.ToString());
            return EStateTreeRunStatus::Failed;
        }
    }

    return EStateTreeRunStatus::Running;
}