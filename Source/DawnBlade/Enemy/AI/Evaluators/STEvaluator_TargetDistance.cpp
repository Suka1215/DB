// STEvaluator_TargetDistance.cpp
#include "STEvaluator_TargetDistance.h"

#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/Data/DEnemyData.h"
#include "DrawDebugHelpers.h"
#include "Enemy/AI/DBAIController_Enemy.h"

USTEvaluator_TargetDistance::USTEvaluator_TargetDistance()
{
    AttackRange = 300.0f;
    AggroRange = 1000.0f;
    bCheckLineOfSight = true;
}

void USTEvaluator_TargetDistance::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    Super::Tick(Context, DeltaTime);

    ADBAIController_Enemy* AIController = GetAIController(Context);
    ADBEnemyCharacter* Enemy = GetEnemyCharacter(Context);
    
    if (!AIController || !Enemy)
    {
        DistanceToTarget = 0.0f;
        bInAttackRange = false;
        bInAggroRange = false;
        bHasLineOfSight = false;
        return;
    }

    AActor* Target = AIController->GetTargetActor();

    if (!Target)
    {
        DistanceToTarget = 0.0f;
        bInAttackRange = false;
        bInAggroRange = false;
        bHasLineOfSight = false;
        return;
    }

    // Calculate distance
    FVector EnemyLocation = Enemy->GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    DistanceToTarget = FVector::Dist(EnemyLocation, TargetLocation);

    // Update last known target location
    AIController->UpdateLastKnownTargetLocation();

    // Get ranges from enemy data if available
    if (Enemy->GetEnemyData())
    {
        UDEnemyData* EnemyData = Enemy->GetEnemyData();
        
        if (EnemyData->AttackPatterns.Num() > 0)
        {
            AttackRange = EnemyData->AttackPatterns[0].AttackRange;
        }
        
        AggroRange = EnemyData->AggroRange;
    }

    // Check ranges
    bInAttackRange = DistanceToTarget <= AttackRange;
    bInAggroRange = DistanceToTarget <= AggroRange;

    // Line of sight check
    if (bCheckLineOfSight)
    {
        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(Enemy);

        FVector Start = EnemyLocation + FVector(0, 0, 50.0f); // Eye height
        FVector End = TargetLocation + FVector(0, 0, 50.0f);

        bHasLineOfSight = !Enemy->GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility,
            CollisionParams
        );

        // Debug draw (optional)
        #if !UE_BUILD_SHIPPING
        if (bHasLineOfSight)
        {
            DrawDebugLine(Enemy->GetWorld(), Start, End, FColor::Green, false, 0.1f);
        }
        else
        {
            DrawDebugLine(Enemy->GetWorld(), Start, End, FColor::Red, false, 0.1f);
        }
        #endif
    }

    // Log for debugging
    UE_LOG(LogTemp, VeryVerbose, TEXT("Distance: %.1f | InAttackRange: %s | InAggroRange: %s | LOS: %s"),
        DistanceToTarget,
        bInAttackRange ? TEXT("YES") : TEXT("NO"),
        bInAggroRange ? TEXT("YES") : TEXT("NO"),
        bHasLineOfSight ? TEXT("YES") : TEXT("NO")
    );
}