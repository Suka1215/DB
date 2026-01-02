// STTask_EngageTarget.cpp
#include "STTask_EngageTarget.h"
#include "AIController.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Components/DBWeaponComponent.h"

EStateTreeRunStatus USTTask_EngageTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    AAIController* ControllerPtr = Controller.Get();
    AActor* TargetPtr = Target.Get();
    
    if (!ControllerPtr || !TargetPtr)
    {
        return EStateTreeRunStatus::Failed;
    }

    ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
    if (!Enemy)
    {
        return EStateTreeRunStatus::Failed;
    }

    UDBWeaponComponent* WeaponComp = Enemy->FindComponentByClass<UDBWeaponComponent>();
    if (!WeaponComp)
    {
        return EStateTreeRunStatus::Failed;
    }

    bWeaponDrawn = false;

    // Draw weapon if not already drawn
    if (!WeaponComp->IsWeaponDrawn())
    {
        WeaponComp->DrawWeapon();
        UE_LOG(LogTemp, Log, TEXT("⚔️ %s drawing weapon to engage %s"), 
            *Enemy->GetName(), *TargetPtr->GetName());
    }
    else
    {
        bWeaponDrawn = true;
    }

    // Move towards target
    ControllerPtr->MoveToActor(TargetPtr, AcceptanceRadius);

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_EngageTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    AAIController* ControllerPtr = Controller.Get();
    AActor* TargetPtr = Target.Get();
    
    if (!ControllerPtr || !TargetPtr)
    {
        return EStateTreeRunStatus::Failed;
    }

    ADBEnemyCharacter* Enemy = Cast<ADBEnemyCharacter>(ControllerPtr->GetPawn());
    if (!Enemy)
    {
        return EStateTreeRunStatus::Failed;
    }

    UDBWeaponComponent* WeaponComp = Enemy->FindComponentByClass<UDBWeaponComponent>();
    if (!WeaponComp)
    {
        return EStateTreeRunStatus::Failed;
    }

    // Check if we're in range
    float DistanceToTarget = FVector::Dist(Enemy->GetActorLocation(), TargetPtr->GetActorLocation());
    if (DistanceToTarget <= EngageDistance)
    {
        // Stop movement
        ControllerPtr->StopMovement();
        UE_LOG(LogTemp, Warning, TEXT("⚔️ %s engaged with %s (Distance: %.1f)"), 
            *Enemy->GetName(), *TargetPtr->GetName(), DistanceToTarget);
        return EStateTreeRunStatus::Succeeded;
    }

    // Continue moving towards target
    ControllerPtr->MoveToActor(TargetPtr, AcceptanceRadius);

    return EStateTreeRunStatus::Running;
}