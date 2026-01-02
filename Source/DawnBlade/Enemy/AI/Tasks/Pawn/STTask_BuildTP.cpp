// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_BuildTP.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Attributes/DBAttr_Combat.h"
#include "Enemy/DBEnemyCharacter.h"

EStateTreeRunStatus USTTask_BuildTP::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy) return EStateTreeRunStatus::Failed;

    bBound = false;

    // Bind to enemy's attack completion event
    if (ADBEnemyCharacter* EnemyChar = Cast<ADBEnemyCharacter>(Enemy))
    {
        EnemyChar->OnAttackCompleted.BindUObject(this, &USTTask_BuildTP::OnAttackHit);
        bBound = true;
        
        UE_LOG(LogTemp, Log, TEXT("⚡ TP building enabled for %s"), *Enemy->GetName());
    }

    return bContinuous ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus USTTask_BuildTP::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!bBound || !Enemy)
    {
        return EStateTreeRunStatus::Failed;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_BuildTP::OnAttackHit()
{
    if (!Enemy) return;

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemy))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            if (UDBAttr_Combat* CombatAttribs = const_cast<UDBAttr_Combat*>(ASC->GetSet<UDBAttr_Combat>()))
            {
                float CurrentTP = CombatAttribs->GetTP();
                float NewTP = FMath::Min(CurrentTP + TPPerHit, 3000.0f); // Max 3000 TP (3 weapon skills)
                
                CombatAttribs->SetTP(NewTP);
                
                if (NewTP >= 1000.0f && CurrentTP < 1000.0f)
                {
                    UE_LOG(LogTemp, Warning, TEXT("⚡✨ %s TP READY! (%.0f)"), *Enemy->GetName(), NewTP);
                }
                else
                {
                    UE_LOG(LogTemp, Log, TEXT("⚡ %s gained TP: %.0f → %.0f"), 
                        *Enemy->GetName(), CurrentTP, NewTP);
                }
            }
        }
    }
}