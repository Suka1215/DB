// STTask_FindNearestTarget.cpp
#include "STTask_FindNearestTarget.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

USTTask_FindNearestTarget::USTTask_FindNearestTarget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldStateChangeOnReselect = false;
}

EStateTreeRunStatus USTTask_FindNearestTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("STTask_FindNearestTarget: Enemy is null"));
        return EStateTreeRunStatus::Failed;
    }

    // Get all player characters in the world
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(Enemy->GetWorld(), ACharacter::StaticClass(), FoundActors);

    AActor* NearestTarget = nullptr;
    float NearestDistance = DetectionRadius;

    FVector EnemyLocation = Enemy->GetActorLocation();

    for (AActor* Actor : FoundActors)
    {
        // Skip self
        if (Actor == Enemy)
        {
            continue;
        }

        // Only target player-controlled characters
        ACharacter* Character = Cast<ACharacter>(Actor);
        if (!Character || !Character->IsPlayerControlled())
        {
            continue;
        }

        // Check distance
        float Distance = FVector::Dist(EnemyLocation, Actor->GetActorLocation());
        if (Distance > DetectionRadius)
        {
            continue;
        }

        // Check line of sight if required
        if (bRequireLineOfSight)
        {
            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(Enemy);
            QueryParams.AddIgnoredActor(Actor);

            bool bHit = Enemy->GetWorld()->LineTraceSingleByChannel(
                HitResult,
                EnemyLocation,
                Actor->GetActorLocation(),
                LineOfSightChannel,
                QueryParams
            );

            // If something blocks LOS, skip this target
            if (bHit && HitResult.GetActor() != Actor)
            {
                continue;
            }
        }

        // Check if this is closer than current nearest
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestTarget = Actor;
        }
    }

    // Set the output
    OutTarget = NearestTarget;

    if (NearestTarget)
    {
        UE_LOG(LogTemp, Log, TEXT("🎯 %s found nearest target: %s at distance %.1f"), 
            *Enemy->GetName(), *NearestTarget->GetName(), NearestDistance);
        return EStateTreeRunStatus::Succeeded;
    }

    UE_LOG(LogTemp, Verbose, TEXT("🔍 %s found no targets within detection radius %.1f"), 
        *Enemy->GetName(), DetectionRadius);
    return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus USTTask_FindNearestTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    // This is a one-shot task, shouldn't tick
    return EStateTreeRunStatus::Running;
}