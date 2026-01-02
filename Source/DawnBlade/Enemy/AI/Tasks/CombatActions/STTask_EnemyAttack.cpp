// STTask_EnemyAttack.cpp
#include "STTask_EnemyAttack.h"
#include "StateTreeExecutionContext.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "AIController.h"

EStateTreeRunStatus USTTask_EnemyAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    // Get AI Controller from context owner
    AAIController* AIControllerBase = Cast<AAIController>(Context.GetOwner());
    ADBAIController_Enemy* AIController = Cast<ADBAIController_Enemy>(AIControllerBase);
    
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No AI Controller in context"));
        return EStateTreeRunStatus::Failed;
    }

    // Get enemy and target
    CachedEnemy = AIController->GetEnemyCharacter();
    CachedTarget = AIController->GetTargetActor();

    if (!CachedEnemy || !CachedTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ Enemy Attack: No enemy or target"));
        return EStateTreeRunStatus::Failed;
    }

    // Face target
    FVector Direction = CachedTarget->GetActorLocation() - CachedEnemy->GetActorLocation();
    Direction.Z = 0.0f;
    if (!Direction.IsNearlyZero())
    {
        CachedEnemy->SetActorRotation(Direction.Rotation());
    }

    // Perform attack
    CachedEnemy->PerformAutoAttack();
    bAttackStarted = true;

    UE_LOG(LogTemp, Log, TEXT("⚔️ %s attacking!"), *CachedEnemy->GetName());

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_EnemyAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!CachedEnemy)
    {
        return EStateTreeRunStatus::Failed;
    }

    // Check if attack is still in progress
    if (bAttackStarted && !CachedEnemy->bIsAttacking)
    {
        UE_LOG(LogTemp, Log, TEXT("✅ Attack completed"));
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_EnemyAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    bAttackStarted = false;
    CachedEnemy = nullptr;
    CachedTarget = nullptr;
}