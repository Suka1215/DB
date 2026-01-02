// DBEnemySpawner.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DBEnemySpawner.generated.h"

class ADBEnemyCharacter;
class UArrowComponent;
class UBillboardComponent;

/**
 * Enemy Spawner - Spawns Blueprint enemy classes at this location.
 * 
 * USAGE:
 * 1. Create a Blueprint child of this spawner OR place it directly
 * 2. Set EnemyClassToSpawn to your enemy Blueprint (e.g., BP_Enemy_Goblin)
 * 3. Configure spawn count, delay, etc.
 * 4. Place in level - enemies spawn on BeginPlay (if bSpawnOnStart is true)
 */
UCLASS()
class DAWNBLADE_API ADBEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    ADBEnemySpawner();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    // ========================================
    // SPAWN SETTINGS
    // ========================================

    /** The Blueprint class to spawn (e.g., BP_Enemy_Goblin) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    TSubclassOf<ADBEnemyCharacter> EnemyClassToSpawn;

    /** Number of enemies to spawn */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "1", ClampMax = "50"))
    int32 SpawnCount = 1;

    /** Radius to spread spawned enemies */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "0", ClampMax = "1000"))
    float SpawnRadius = 200.0f;

    /** Should spawn immediately on game start */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    bool bSpawnOnStart = true;

    /** Delay before first spawn */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "0", ClampMax = "60", EditCondition = "bSpawnOnStart"))
    float InitialSpawnDelay = 0.0f;

    /** If true, respawn enemies when they die */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    bool bRespawnOnDeath = false;

    /** Delay before respawning after death */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "0", ClampMax = "60", EditCondition = "bRespawnOnDeath"))
    float RespawnDelay = 5.0f;

    // ========================================
    // PUBLIC FUNCTIONS
    // ========================================

    /** Manually trigger spawning */
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void SpawnEnemies();

    /** Get all currently alive enemies from this spawner */
    UFUNCTION(BlueprintPure, Category = "Spawner")
    TArray<ADBEnemyCharacter*> GetSpawnedEnemies() const { return SpawnedEnemies; }

    /** Get number of alive enemies */
    UFUNCTION(BlueprintPure, Category = "Spawner")
    int32 GetAliveCount() const { return SpawnedEnemies.Num(); }

protected:
    // ========================================
    // COMPONENTS
    // ========================================

    /** Arrow showing spawn direction */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnDirection;

#if WITH_EDITORONLY_DATA
    /** Billboard for visibility in editor */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBillboardComponent> EditorBillboard;
#endif

    // ========================================
    // INTERNAL
    // ========================================

    UPROPERTY()
    TArray<TObjectPtr<ADBEnemyCharacter>> SpawnedEnemies;

    FTimerHandle SpawnTimerHandle;

    UFUNCTION()
    void OnEnemyDied(ADBEnemyCharacter* DeadEnemy);

    void SpawnSingleEnemy(const FVector& Location, const FRotator& Rotation);
};