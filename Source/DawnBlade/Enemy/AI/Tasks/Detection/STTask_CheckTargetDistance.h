// Copyright DawnBlade. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_CheckTargetDistance.generated.h"

/**
 * State Tree Task: Check Target Distance (Blueprint Base)
 * 
 * Checks distance to target and outputs boolean flags for different ranges.
 * Useful for determining combat behavior based on distance.
 * 
 * Distance Categories:
 * - Melee Range: Within melee attack distance
 * - Close Range: Within engagement range but outside melee
 * - Medium Range: Within detection but requires movement
 * - Far Range: At edge of detection or beyond
 * 
 * Note: Can also use STEvaluator_TargetDistance for this,
 * but task version provides more explicit control in State Tree.
 * 
 * Usage:
 * - Set TargetActor to check
 * - Configure distance thresholds
 * - Use output booleans in State Tree transitions
 * 
 * Outputs:
 * - Distance: Actual distance to target
 * - bInMeleeRange: True if within melee range
 * - bInCloseRange: True if in close range
 * - bInMediumRange: True if in medium range
 * - bInFarRange: True if far or beyond detection
 */
UCLASS(Blueprintable, meta = (DisplayName = "Check Target Distance"))
class DAWNBLADE_API USTTask_CheckTargetDistance : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_CheckTargetDistance(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UStateTreeTaskBlueprintBase Interface
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
    //~ End UStateTreeTaskBlueprintBase Interface

public:
    //------------------------------
    // INPUTS
    //------------------------------

    /** The target actor to check distance to */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<AActor> TargetActor;

    /** Melee attack range (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Thresholds", meta = (ClampMin = "0"))
    float MeleeRange = 200.0f;

    /** Close combat range (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Thresholds", meta = (ClampMin = "0"))
    float CloseRange = 500.0f;

    /** Medium range (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Thresholds", meta = (ClampMin = "0"))
    float MediumRange = 1500.0f;

    /** Far range threshold (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Thresholds", meta = (ClampMin = "0"))
    float FarRange = 3000.0f;

    /** Whether to use 2D distance (ignore Z axis) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bUse2DDistance = true;

    //------------------------------
    // OUTPUTS
    //------------------------------

    /** Actual distance to target (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    float Distance = 0.0f;

    /** True if within melee range */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bInMeleeRange = false;

    /** True if in close range (but not melee) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bInCloseRange = false;

    /** True if in medium range */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bInMediumRange = false;

    /** True if in far range or beyond */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bInFarRange = false;

    /** True if target is out of all ranges */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bOutOfRange = false;

private:
    //------------------------------
    // HELPER FUNCTIONS
    //------------------------------

    /** Calculate distance to target */
    float CalculateDistance(AActor* OwnerActor) const;

    /** Set output flags based on distance */
    void SetDistanceFlags();
};