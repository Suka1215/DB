// DBEnemySpawner.cpp
#include "DBEnemySpawner.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

ADBEnemySpawner::ADBEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;  // ✅ ADD THIS

    // Create root scene component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(RootComponent);

    // Arrow for spawn direction
    SpawnDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));
    SpawnDirection->SetupAttachment(RootComponent);
    SpawnDirection->SetArrowColor(FColor::Green);
    SpawnDirection->ArrowSize = 1.5f;

#if WITH_EDITORONLY_DATA
    // Editor billboard for visibility
    EditorBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorBillboard"));
    EditorBillboard->SetupAttachment(RootComponent);
#endif
}

void ADBEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    // Only spawn on server
    if (!HasAuthority())
    {
        return;
    }

    if (bSpawnOnStart)
    {
        if (InitialSpawnDelay > 0.0f)
        {
            GetWorld()->GetTimerManager().SetTimer(
                SpawnTimerHandle,
                this,
                &ADBEnemySpawner::SpawnEnemies,
                InitialSpawnDelay,
                false);
        }
        else
        {
            SpawnEnemies();
        }
    }
}

void ADBEnemySpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}

void ADBEnemySpawner::SpawnEnemies()
{
    if (!HasAuthority())
    {
        return;
    }

    if (!EnemyClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s: No EnemyClassToSpawn assigned!"), *GetName());
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FRotator SpawnRotation = SpawnDirection ? SpawnDirection->GetComponentRotation() : GetActorRotation();
    FVector BaseLocation = GetActorLocation();

    // Use NavMesh if available for better placement
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

    for (int32 i = 0; i < SpawnCount; i++)
    {
        FVector SpawnLocation = BaseLocation;

        // Spread enemies in a circle if spawning multiple
        if (SpawnCount > 1 && SpawnRadius > 0.0f)
        {
            if (NavSys)
            {
                // Try to find a valid NavMesh location
                FNavLocation NavLoc;
                if (NavSys->GetRandomReachablePointInRadius(BaseLocation, SpawnRadius, NavLoc))
                {
                    SpawnLocation = NavLoc.Location;
                }
            }
            else
            {
                // Fallback: simple circle placement
                float Angle = (360.0f / SpawnCount) * i;
                float Rad = FMath::DegreesToRadians(Angle);
                SpawnLocation.X += FMath::Cos(Rad) * SpawnRadius;
                SpawnLocation.Y += FMath::Sin(Rad) * SpawnRadius;
            }
        }

        SpawnSingleEnemy(SpawnLocation, SpawnRotation);
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ %s spawned %d enemies"), *GetName(), SpawnedEnemies.Num());
}

void ADBEnemySpawner::SpawnSingleEnemy(const FVector& Location, const FRotator& Rotation)
{
    UWorld* World = GetWorld();
    if (!World || !EnemyClassToSpawn) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;

    ADBEnemyCharacter* NewEnemy = World->SpawnActor<ADBEnemyCharacter>(
        EnemyClassToSpawn,
        Location,
        Rotation,
        SpawnParams);

    if (NewEnemy)
    {
        NewEnemy->OnEnemyDied.AddDynamic(this, &ADBEnemySpawner::OnEnemyDied);
        SpawnedEnemies.Add(NewEnemy);

        UE_LOG(LogTemp, Log, TEXT("  → Spawned: %s at %s"),
            *NewEnemy->GetName(),
            *Location.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to spawn enemy at %s"), *Location.ToString());
    }
}

void ADBEnemySpawner::OnEnemyDied(ADBEnemyCharacter* DeadEnemy)
{
    if (!DeadEnemy) return;

    SpawnedEnemies.Remove(DeadEnemy);

    UE_LOG(LogTemp, Log, TEXT("💀 %s: Enemy died, %d remaining"),
        *GetName(), SpawnedEnemies.Num());

    // Respawn if enabled
    if (bRespawnOnDeath && HasAuthority())
    {
        FVector RespawnLocation = GetActorLocation();
        FRotator RespawnRotation = SpawnDirection ? SpawnDirection->GetComponentRotation() : GetActorRotation();

        // Use a lambda to capture spawn location
        FTimerHandle RespawnHandle;
        GetWorld()->GetTimerManager().SetTimer(
            RespawnHandle,
            [this, RespawnLocation, RespawnRotation]()
            {
                SpawnSingleEnemy(RespawnLocation, RespawnRotation);
            },
            RespawnDelay,
            false);
    }
}