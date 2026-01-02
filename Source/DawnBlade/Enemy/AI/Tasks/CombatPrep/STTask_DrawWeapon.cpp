#include "STTask_DrawWeapon.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "Components/StateTreeComponent.h"

USTTask_DrawWeapon::USTTask_DrawWeapon(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldCallTick = true;
    bShouldCopyBoundPropertiesOnTick = false;
    bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus USTTask_DrawWeapon::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    UE_LOG(LogTemp, Error, TEXT("🔵 DrawWeapon EnterState called"));

    if (!Actor)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ DrawWeapon: No Actor (Context binding failed)"));
        return EStateTreeRunStatus::Failed;
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ DrawWeapon: Found enemy %s"), *Actor->GetName());

    bMontageFinished = false;

    // Set weapon drawn tag
    if (UDBAIStateComponent* TagState = Actor->FindComponentByClass<UDBAIStateComponent>())
    {
        TagState->SetWeapon_Drawn();
    }

    // Play montage if provided
    if (DrawMontage)
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
            {
                if (!AnimInst->OnMontageStarted.IsAlreadyBound(this, &USTTask_DrawWeapon::HandleMontageStarted))
                {
                    AnimInst->OnMontageStarted.AddDynamic(this, &USTTask_DrawWeapon::HandleMontageStarted);
                }
                
                if (!AnimInst->OnMontageEnded.IsAlreadyBound(this, &USTTask_DrawWeapon::HandleMontageEnded))
                {
                    AnimInst->OnMontageEnded.AddDynamic(this, &USTTask_DrawWeapon::HandleMontageEnded);
                }
                
                if (Actor->HasAuthority())
                {
                    Actor->MulticastPlayMontage(DrawMontage, 1.0f);
                }
                float Duration = DrawMontage->GetPlayLength();
                
                if (Duration > 0.0f)
                {
                    UE_LOG(LogTemp, Error, TEXT("🏃 DrawWeapon returning RUNNING (Duration: %.2fs)"), Duration);
                    return EStateTreeRunStatus::Running;
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("❌ DrawWeapon: Failed to play montage"));
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ DrawWeapon: No DrawMontage assigned, succeeding immediately"));
    }

    // No montage or failed to play - succeed immediately
    return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus USTTask_DrawWeapon::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    UE_LOG(LogTemp, Error, TEXT("🔄 DrawWeapon TICK called (bMontageFinished=%s)"),
        bMontageFinished ? TEXT("TRUE") : TEXT("FALSE"));

    if (bMontageFinished)
    {
        // ✅ Send Weapon.Drawn event when task completes
        if (AAIController* AIController = Cast<AAIController>(Actor->GetController()))
        {
            if (UStateTreeComponent* StateTreeComp = AIController->FindComponentByClass<UStateTreeComponent>())
            {
               UE_LOG(LogTemp, Error, TEXT("✅ DrawWeapon TICK returning SUCCEEDED"));
               return EStateTreeRunStatus::Succeeded;
            }
        }
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_DrawWeapon::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    UE_LOG(LogTemp, Warning, TEXT("🛑 DrawWeapon ExitState called"));

    if (Actor)
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
            {
                if (AnimInst->OnMontageStarted.IsAlreadyBound(this, &USTTask_DrawWeapon::HandleMontageStarted))
                {
                    AnimInst->OnMontageStarted.RemoveDynamic(this, &USTTask_DrawWeapon::HandleMontageStarted);
                }
                
                if (AnimInst->OnMontageEnded.IsAlreadyBound(this, &USTTask_DrawWeapon::HandleMontageEnded))
                {
                    AnimInst->OnMontageEnded.RemoveDynamic(this, &USTTask_DrawWeapon::HandleMontageEnded);
                }

                if (AnimInst->Montage_IsPlaying(DrawMontage))
                {
                    AnimInst->Montage_Stop(0.2f, DrawMontage);
                }
            }
        }
    }
}

void USTTask_DrawWeapon::HandleMontageStarted(UAnimMontage* TheMontage)
{
    UE_LOG(LogTemp, Warning, TEXT("🎬 Montage Started: %s"), *TheMontage->GetName());
}

void USTTask_DrawWeapon::HandleMontageEnded(UAnimMontage* TheMontage, bool bInterrupted)
{
    UE_LOG(LogTemp, Error, TEXT("✅ Montage Ended: %s (Interrupted: %s)"), 
        *TheMontage->GetName(), 
        bInterrupted ? TEXT("YES") : TEXT("NO"));
    
    bMontageFinished = true;
}
