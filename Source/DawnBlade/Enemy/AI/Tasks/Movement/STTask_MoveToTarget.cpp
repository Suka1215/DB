#include "STTask_MoveToTarget.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Enemy/AI/DBAIController_Enemy.h"

USTTask_MoveToTarget::USTTask_MoveToTarget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

EStateTreeRunStatus USTTask_MoveToTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToTarget: No owner actor!"));
        return EStateTreeRunStatus::Failed;
    }

    ADBEnemyCharacter* EnemyChar = Cast<ADBEnemyCharacter>(OwnerActor);
    if (!EnemyChar)
    {
        AAIController* AIController = Cast<AAIController>(OwnerActor);
        if (AIController)
        {
            EnemyChar = Cast<ADBEnemyCharacter>(AIController->GetPawn());
        }
    }

    if (!EnemyChar)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToTarget: No enemy character!"));
        return EStateTreeRunStatus::Failed;
    }

    ADBAIController_Enemy* EnemyAI = Cast<ADBAIController_Enemy>(EnemyChar->GetController());
    if (!EnemyAI)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToTarget: No enemy AI controller!"));
        return EStateTreeRunStatus::Failed;
    }

    AActor* TargetActor = EnemyAI->GetCurrentTarget();
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToTarget: AI Controller has no target!"));
        return EStateTreeRunStatus::Failed;
    }

    // Start moving to target
    EnemyAI->MoveToActor(TargetActor, AcceptableRadius);

    float Distance = FVector::Dist(EnemyChar->GetActorLocation(), TargetActor->GetActorLocation());
    
    UE_LOG(LogTemp, Warning, TEXT("🚶 %s moving to %s (Range: %.1f, Distance: %.1f)"), 
        *EnemyChar->GetName(), 
        *TargetActor->GetName(), 
        AcceptableRadius,
        Distance);

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_MoveToTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
    if (!OwnerActor)
    {
        return EStateTreeRunStatus::Failed;
    }

    ADBEnemyCharacter* EnemyChar = Cast<ADBEnemyCharacter>(OwnerActor);
    if (!EnemyChar)
    {
        AAIController* AIController = Cast<AAIController>(OwnerActor);
        if (AIController)
        {
            EnemyChar = Cast<ADBEnemyCharacter>(AIController->GetPawn());
        }
    }

    if (!EnemyChar)
    {
        return EStateTreeRunStatus::Failed;
    }

    ADBAIController_Enemy* EnemyAI = Cast<ADBAIController_Enemy>(EnemyChar->GetController());
    if (!EnemyAI)
    {
        return EStateTreeRunStatus::Failed;
    }

    AActor* TargetActor = EnemyAI->GetCurrentTarget();
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ MoveToTarget: Lost target during movement"));
        return EStateTreeRunStatus::Failed;
    }

    // ✅ Continuously update movement to follow moving target
    float Distance = FVector::Dist(EnemyChar->GetActorLocation(), TargetActor->GetActorLocation());
    
    // ✅ Check if target moved significantly - update path
    static float LastUpdateTime = 0.0f;
    float CurrentTime = Context.GetWorld()->GetTimeSeconds();
    
    if (CurrentTime - LastUpdateTime > 0.5f) // Update path every 0.5 seconds
    {
        EnemyAI->MoveToActor(TargetActor, AcceptableRadius);
        LastUpdateTime = CurrentTime;
    }

    // Check if we've reached the target
    if (Distance <= AcceptableRadius)
    {
        UE_LOG(LogTemp, Log, TEXT("✅ %s reached target (Distance: %.1f)"), *EnemyChar->GetName(), Distance);
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_MoveToTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
    if (!OwnerActor)
    {
        return;
    }

    ADBEnemyCharacter* EnemyChar = Cast<ADBEnemyCharacter>(OwnerActor);
    if (!EnemyChar)
    {
        AAIController* AIController = Cast<AAIController>(OwnerActor);
        if (AIController)
        {
            EnemyChar = Cast<ADBEnemyCharacter>(AIController->GetPawn());
        }
    }

    if (!EnemyChar)
    {
        return;
    }

    AAIController* AIController = Cast<AAIController>(EnemyChar->GetController());
    if (AIController)
    {
        AIController->StopMovement();
        UE_LOG(LogTemp, Log, TEXT("🛑 %s stopped moving to target"), *EnemyChar->GetName());
    }
}