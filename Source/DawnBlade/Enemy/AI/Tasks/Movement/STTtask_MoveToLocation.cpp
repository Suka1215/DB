#include "STTtask_MoveToLocation.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Components/StateTreeComponent.h"

EStateTreeRunStatus USTTask_MoveToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToLocation: No Enemy!"));
        return EStateTreeRunStatus::Failed;
    }

    ADBAIController_Enemy* AIController = Cast<ADBAIController_Enemy>(Enemy->GetController());
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MoveToLocation: No AI Controller on %s!"), *Enemy->GetName());
        return EStateTreeRunStatus::Failed;
    }

    // Ensure target location is valid on NavMesh
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Enemy->GetWorld());
    if (NavSys)
    {
        FNavLocation Projected;
        if (NavSys->ProjectPointToNavigation(TargetLocation, Projected, FVector(500.f, 500.f, 1000.f)))
        {
            TargetLocation = Projected.Location;
            UE_LOG(LogTemp, Warning, TEXT("📍 Projected Move Target to NavMesh: %s"), *TargetLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Failed to project move target to NavMesh: %s"), *TargetLocation.ToString());
            return EStateTreeRunStatus::Failed;
        }
    }

    // Draw debug
    DrawDebugLine(Enemy->GetWorld(), Enemy->GetActorLocation(), TargetLocation, FColor::Green, false, 5.0f, 0, 4.0f);
    DrawDebugSphere(Enemy->GetWorld(), TargetLocation, 50.f, 12, FColor::Green, false, 5.0f);

    UE_LOG(LogTemp, Warning, TEXT("🎯 Attempting move from %s to %s"),
        *Enemy->GetActorLocation().ToString(), *TargetLocation.ToString());

    // Construct the move request
    FAIMoveRequest MoveRequest(TargetLocation);
    MoveRequest.SetAcceptanceRadius(AcceptableRadius);
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetProjectGoalLocation(true);
    MoveRequest.SetAllowPartialPath(true);

    FPathFollowingRequestResult Result = AIController->MoveTo(MoveRequest);

    if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        MoveRequestID = Result.MoveId;
        UE_LOG(LogTemp, Warning, TEXT("🚶 %s moving to location: %s"),
            *Enemy->GetName(), *TargetLocation.ToString());
        return EStateTreeRunStatus::Running;
    }

    UE_LOG(LogTemp, Error, TEXT("❌ Failed to start movement for %s (Result: %d)"),
        *Enemy->GetName(), (int32)Result.Code);
    return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus USTTask_MoveToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!Enemy)
        return EStateTreeRunStatus::Failed;

    AAIController* AIController = Cast<AAIController>(Enemy->GetController());
    if (!AIController)
        return EStateTreeRunStatus::Failed;

    EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

    switch (Status)
    {
    case EPathFollowingStatus::Moving:
        return EStateTreeRunStatus::Running;

    case EPathFollowingStatus::Idle:
        UE_LOG(LogTemp, Log, TEXT("✅ %s reached destination"), *Enemy->GetName());
        return EStateTreeRunStatus::Succeeded;

    case EPathFollowingStatus::Paused:
        UE_LOG(LogTemp, Warning, TEXT("⏸️ Movement paused for %s"), *Enemy->GetName());
        return EStateTreeRunStatus::Running;

    default:
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Movement failed for %s (Status: %d)"),
            *Enemy->GetName(), (int32)Status);
        return EStateTreeRunStatus::Failed;
    }
}

void USTTask_MoveToLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
        return;

    if (AAIController* AIController = Cast<AAIController>(Enemy->GetController()))
    {
        AIController->StopMovement();
        UE_LOG(LogTemp, Log, TEXT("🛑 Stopped movement for %s"), *Enemy->GetName());
    }
}
