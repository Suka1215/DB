// Copyright DawnBlade. All Rights Reserved.

#include "STTask_CastMagic.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"
#include "GameFramework/Character.h"

// Define native gameplay tags
FGameplayTag USTTask_CastMagic::Tag_State_AI_Casting = FGameplayTag();
FGameplayTag USTTask_CastMagic::Tag_State_Combat_Casting = FGameplayTag();

USTTask_CastMagic::USTTask_CastMagic(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize tags on first use
    if (!Tag_State_AI_Casting.IsValid())
    {
        Tag_State_AI_Casting = FGameplayTag::RequestGameplayTag(FName("State.AI.Casting"));
        Tag_State_Combat_Casting = FGameplayTag::RequestGameplayTag(FName("State.Combat.Casting"));
    }
}

EStateTreeRunStatus USTTask_CastMagic::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ STTask_CastMagic: No enemy reference"));
        return EStateTreeRunStatus::Failed;
    }

    if (!SpellTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ STTask_CastMagic: Invalid spell tag"));
        return EStateTreeRunStatus::Failed;
    }

    // Check MP
    if (!HasSufficientMana())
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ %s: Insufficient MP for %s (Need: %d, Have: %d)"),
            *Enemy->GetName(), *SpellTag.ToString(), ManaCost, GetCurrentMana());
        bCastSuccessful = false;
        return EStateTreeRunStatus::Failed;
    }

    // Start casting
    StartCasting();

    ElapsedCastTime = 0.0f;
    bIsCasting = true;
    bCastSuccessful = false;
    bCastInterrupted = false;

    UE_LOG(LogTemp, Log, TEXT("🔮 %s → CASTING %s (MP: %d, Time: %.1fs)"),
        *Enemy->GetName(), *SpellTag.ToString(), ManaCost, CastTime);

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_CastMagic::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!bIsCasting || !Enemy) return EStateTreeRunStatus::Failed;

    // TODO: Check for interrupt conditions (damage, stun, silence)
    // if (bCanBeInterrupted && WasDamaged()) { InterruptCast(); return Failed; }

    // Face target while casting
    if (bFaceTargetWhileCasting && TargetActor)
    {
        FVector DirectionToTarget = TargetActor->GetActorLocation() - Enemy->GetActorLocation();
        DirectionToTarget.Z = 0.0f;
        if (!DirectionToTarget.IsNearlyZero())
        {
            FRotator TargetRotation = DirectionToTarget.Rotation();
            FRotator NewRotation = FMath::RInterpTo(Enemy->GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
            Enemy->SetActorRotation(NewRotation);
        }
    }

    // Update cast timer
    ElapsedCastTime += DeltaTime;

    // Check if cast complete
    if (ElapsedCastTime >= CastTime)
    {
        ConsumeMana();
        CompleteCast();
        bCastSuccessful = true;
        bIsCasting = false;

        UE_LOG(LogTemp, Log, TEXT("✅ %s → Cast Complete: %s"), *Enemy->GetName(), *SpellTag.ToString());
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_CastMagic::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (bIsCasting && Enemy)
    {
        InterruptCast();
        bCastInterrupted = true;
        UE_LOG(LogTemp, Warning, TEXT("⚠️ %s → Cast Interrupted"), *Enemy->GetName());
    }

    ElapsedCastTime = 0.0f;
    bIsCasting = false;
}

bool USTTask_CastMagic::HasSufficientMana() const
{
    int32 CurrentMana = GetCurrentMana();
    return CurrentMana >= ManaCost;
}

void USTTask_CastMagic::ConsumeMana()
{
    int32 CurrentMana = GetCurrentMana();
    int32 NewMana = FMath::Max(0, CurrentMana - ManaCost);
    SetCurrentMana(NewMana);
}

void USTTask_CastMagic::StartCasting()
{
    if (!Enemy) return;

    // Set casting tags
    if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
    {
        TagState->SetAI_Casting();
        TagState->SetCombat_Casting();
    }

    // Play casting animation
    if (CastMontage)
    {
        if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
        {
            AnimInstance->Montage_Play(CastMontage, 1.0f);
        }
    }

    // TODO: Spawn casting VFX
    // TODO: Play casting sound
}

void USTTask_CastMagic::CompleteCast()
{
    if (!Enemy) return;

    // Remove casting tags
    if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
    {
        TagState->RemoveAI_Casting();
        TagState->RemoveCombat_Casting();
    }

    // TODO: Fire spell effect based on SpellTag
    // TODO: Apply damage/healing to TargetActor
    // TODO: Spawn spell VFX
    // TODO: Play spell sound
    
    UE_LOG(LogTemp, Log, TEXT("🎯 %s → Spell Effect: %s on %s"),
        *Enemy->GetName(),
        *SpellTag.ToString(),
        TargetActor ? *TargetActor->GetName() : TEXT("None"));
}

void USTTask_CastMagic::InterruptCast()
{
    if (!Enemy) return;

    // Remove casting tags
    if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
    {
        TagState->RemoveAI_Casting();
        TagState->RemoveCombat_Casting();
    }

    // Stop animation
    if (CastMontage)
    {
        if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
        {
            if (AnimInstance->Montage_IsPlaying(CastMontage))
            {
                AnimInstance->Montage_Stop(0.2f, CastMontage);
            }
        }
    }

    // TODO: Spawn interrupt VFX
    // TODO: Play interrupt sound
}

int32 USTTask_CastMagic::GetCurrentMana() const
{
    // TODO: Integrate with your attribute component
    // Example:
    // if (UDBAttributeComponent* Attrs = Enemy->FindComponentByClass<UDBAttributeComponent>())
    // {
    //     return Attrs->GetCurrentMana();
    // }
    return 1000; // Placeholder
}

void USTTask_CastMagic::SetCurrentMana(int32 NewMana)
{
    // TODO: Integrate with your attribute component
    // Example:
    // if (UDBAttributeComponent* Attrs = Enemy->FindComponentByClass<UDBAttributeComponent>())
    // {
    //     Attrs->SetCurrentMana(NewMana);
    // }
}