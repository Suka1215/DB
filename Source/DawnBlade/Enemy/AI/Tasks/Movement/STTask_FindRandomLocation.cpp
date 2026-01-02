#include "STTask_FindRandomLocation.h"
#include "Enemy/DBEnemyCharacter.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

EStateTreeRunStatus USTTask_FindRandomLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ FindRandomLocation: Enemy is null!"));
        return EStateTreeRunStatus::Failed;
    }

    FVector Origin = Enemy->GetActorLocation();
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Enemy->GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Navigation System!"));
        return EStateTreeRunStatus::Failed;
    }

    // Project the origin onto the NavMesh to ensure valid layer
    FNavLocation ProjectedOrigin;
    if (!NavSys->ProjectPointToNavigation(Origin, ProjectedOrigin, FVector(500.f, 500.f, 1000.f)))
    {
        UE_LOG(LogTemp, Error, TEXT("⚠️ Could not project origin %s onto NavMesh"), *Origin.ToString());
        return EStateTreeRunStatus::Failed;
    }

    Origin = ProjectedOrigin.Location;

    // Try to find a reachable point near the origin
    for (int32 i = 0; i < 20; i++)
    {
        FNavLocation NavLocation;
        if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, NavLocation))
        {
            FNavLocation Projected;
            if (NavSys->ProjectPointToNavigation(NavLocation.Location, Projected, FVector(500.f, 500.f, 1000.f)))
            {
                float ZDiff = FMath::Abs(Origin.Z - Projected.Location.Z);
                if (ZDiff < 100.0f)
                {
                    RandomLocation = Projected.Location;

                    UE_LOG(LogTemp, Warning, TEXT("✅ Found patrol point (NavMesh): %s (Z-diff: %.1f, try %d)"),
                        *RandomLocation.ToString(), ZDiff, i + 1);

                    DrawDebugSphere(Enemy->GetWorld(), RandomLocation, 50.f, 12, FColor::Cyan, false, 5.0f);
                    return EStateTreeRunStatus::Succeeded;
                }
            }
        }
    }

    // Fallback – simple circle search at same Z height
    float Angle = FMath::RandRange(0.f, 2 * PI);
    float Dist = FMath::RandRange(SearchRadius * 0.3f, SearchRadius);
    RandomLocation = Origin + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 0.f);

    UE_LOG(LogTemp, Warning, TEXT("⚠️ Using fallback patrol point: %s"), *RandomLocation.ToString());
    DrawDebugSphere(Enemy->GetWorld(), RandomLocation, 50.f, 12, FColor::Yellow, false, 5.0f);

    return EStateTreeRunStatus::Succeeded;
}
