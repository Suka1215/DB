#include "STTask_PlayMontage.h"
#include "Enemy/DBEnemyCharacter.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"

USTTask_PlayMontage::USTTask_PlayMontage(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

EStateTreeRunStatus USTTask_PlayMontage::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    // ✅ Get character from context, not from property
    AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ PlayMontage: No owner actor"));
        return EStateTreeRunStatus::Failed;
    }

    // Try to get character - might be owner or might need to get from controller
    ACharacter* Character = Cast<ACharacter>(OwnerActor);
    if (!Character)
    {
        AAIController* AIController = Cast<AAIController>(OwnerActor);
        if (AIController)
        {
            Character = Cast<ACharacter>(AIController->GetPawn());
        }
    }

    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ PlayMontage: No character found"));
        return EStateTreeRunStatus::Failed;
    }

    if (!Montage)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ PlayMontage: No montage assigned"));
        return EStateTreeRunStatus::Failed;
    }

    bMontageFinished = false;

    UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ PlayMontage: No anim instance"));
        return EStateTreeRunStatus::Failed;
    }

    // ✅ Play montage
    float Duration = AnimInstance->Montage_Play(Montage, PlayRate);
    
    if (Duration <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ PlayMontage: Failed to play %s"), *Montage->GetName());
        return EStateTreeRunStatus::Failed;
    }

    // ✅ Bind end delegate
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &USTTask_PlayMontage::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

    UE_LOG(LogTemp, Warning, TEXT("🎬 Playing montage: %s (Duration: %.2fs)"), 
        *Montage->GetName(), Duration);

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_PlayMontage::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (bMontageFinished)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Montage task succeeded"));
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_PlayMontage::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    bMontageFinished = false;
    
    UE_LOG(LogTemp, Log, TEXT("🛑 PlayMontage task exited"));
}

void USTTask_PlayMontage::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
    bMontageFinished = !bInterrupted;
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Montage completed: %s (interrupted: %s)"), 
        *InMontage->GetName(),
        bInterrupted ? TEXT("YES") : TEXT("NO"));
}