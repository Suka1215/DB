#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DBEnemyStateTreeContext.generated.h"

UCLASS(BlueprintType)
class DAWNBLADE_API UDBEnemyStateTreeContext : public UObject
{
    GENERATED_BODY()

public:
    // ========================================
    // COMBAT TARGET
    // ========================================
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> CurrentTarget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector LastKnownTargetLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DistanceToTarget = 0.0f;

    // ========================================
    // PATROL
    // ========================================
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    FVector PatrolLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadius = 1000.0f;

    // ========================================
    // STATE FLAGS
    // ========================================
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsEngaged = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bWeaponDrawn = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsPatrolling = false;

    // ========================================
    // HELPERS
    // ========================================
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool HasValidTarget() const { return CurrentTarget != nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ClearTarget()
    {
        CurrentTarget = nullptr;
        DistanceToTarget = 0.0f;
    }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void UpdateDistanceToTarget(const FVector& MyLocation)
    {
        if (CurrentTarget)
        {
            DistanceToTarget = FVector::Dist(MyLocation, CurrentTarget->GetActorLocation());
        }
        else
        {
            DistanceToTarget = 0.0f;
        }
    }
};