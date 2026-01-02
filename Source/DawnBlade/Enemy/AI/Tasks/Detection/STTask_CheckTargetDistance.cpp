// Copyright DawnBlade. All Rights Reserved.

#include "STTask_CheckTargetDistance.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"

USTTask_CheckTargetDistance::USTTask_CheckTargetDistance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default values set in header
}

EStateTreeRunStatus USTTask_CheckTargetDistance::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    // Reset outputs
    Distance = 0.0f;
    bInMeleeRange = false;
    bInCloseRange = false;
    bInMediumRange = false;
    bInFarRange = false;
    bOutOfRange = false;

    // Validate target
    if (!TargetActor || !IsValid(TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("STTask_CheckTargetDistance: Invalid target"));
        bOutOfRange = true;
        return EStateTreeRunStatus::Failed;
    }

    // Get owner actor
    AAIController* AIController = Cast<AAIController>(Context.GetOwner());
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("STTask_CheckTargetDistance: No AI Controller found"));
        return EStateTreeRunStatus::Failed;
    }

    AActor* OwnerActor = AIController->GetPawn();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("STTask_CheckTargetDistance: No pawn found"));
        return EStateTreeRunStatus::Failed;
    }

    // Calculate distance
    Distance = CalculateDistance(OwnerActor);

    // Set distance flags
    SetDistanceFlags();

    // Log result
    FString RangeCategory;
    if (bInMeleeRange) RangeCategory = TEXT("Melee");
    else if (bInCloseRange) RangeCategory = TEXT("Close");
    else if (bInMediumRange) RangeCategory = TEXT("Medium");
    else if (bInFarRange) RangeCategory = TEXT("Far");
    else RangeCategory = TEXT("Out of Range");

    UE_LOG(LogTemp, Verbose, TEXT("STTask_CheckTargetDistance: Target at %.1f cm (%s range)"),
        Distance, *RangeCategory);

    return EStateTreeRunStatus::Succeeded;
}

float USTTask_CheckTargetDistance::CalculateDistance(AActor* OwnerActor) const
{
    if (!OwnerActor || !TargetActor)
    {
        return 0.0f;
    }

    FVector OwnerLocation = OwnerActor->GetActorLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();

    if (bUse2DDistance)
    {
        // Ignore Z axis for 2D distance
        OwnerLocation.Z = 0.0f;
        TargetLocation.Z = 0.0f;
    }

    return FVector::Dist(OwnerLocation, TargetLocation);
}

void USTTask_CheckTargetDistance::SetDistanceFlags()
{
    // Reset all flags
    bInMeleeRange = false;
    bInCloseRange = false;
    bInMediumRange = false;
    bInFarRange = false;
    bOutOfRange = false;

    // Set appropriate flag based on distance
    if (Distance <= MeleeRange)
    {
        bInMeleeRange = true;
    }
    else if (Distance <= CloseRange)
    {
        bInCloseRange = true;
    }
    else if (Distance <= MediumRange)
    {
        bInMediumRange = true;
    }
    else if (Distance <= FarRange)
    {
        bInFarRange = true;
    }
    else
    {
        bOutOfRange = true;
    }
}