// Copyright DawnBlade. All Rights Reserved.

#include "STTask_SheatheWeapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"
#include "GameFramework/Character.h"

// Define native gameplay tags
FGameplayTag USTTask_SheatheWeapon::Tag_State_Weapon_Drawn = FGameplayTag();
FGameplayTag USTTask_SheatheWeapon::Tag_State_Weapon_Sheathed = FGameplayTag();

USTTask_SheatheWeapon::USTTask_SheatheWeapon(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    if (!Tag_State_Weapon_Drawn.IsValid())
    {
        Tag_State_Weapon_Drawn = FGameplayTag::RequestGameplayTag(FName("Weapon.Drawn"));
        Tag_State_Weapon_Sheathed = FGameplayTag::RequestGameplayTag(FName("Weapon.Sheathed"));
    }
}

EStateTreeRunStatus USTTask_SheatheWeapon::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ STTask_SheatheWeapon: No enemy reference"));
        return EStateTreeRunStatus::Failed;
    }

    StartSheathe();

    ElapsedTime = 0.0f;
    bIsSheathing = true;
    bSheatheSuccessful = false;

    UE_LOG(LogTemp, Log, TEXT("🗡️ %s → SHEATHING Weapon"), *Enemy->GetName());

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_SheatheWeapon::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    if (!bIsSheathing || !Enemy) return EStateTreeRunStatus::Failed;

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= SheatheTime)
    {
        CompleteSheathe();
        bSheatheSuccessful = true;
        bIsSheathing = false;
        
        UE_LOG(LogTemp, Log, TEXT("✅ %s → Weapon Sheathed"), *Enemy->GetName());
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Running;
}

void USTTask_SheatheWeapon::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (bIsSheathing && Enemy)
    {
        CompleteSheathe();
    }

    ElapsedTime = 0.0f;
    bIsSheathing = false;
}

void USTTask_SheatheWeapon::StartSheathe()
{
    if (!Enemy) return;

    // Play sheathe animation
    if (SheatheMontage)
    {
        if (UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance())
        {
            AnimInstance->Montage_Play(SheatheMontage, 1.0f);
        }
    }

    // TODO: Play sheathe sound
}

void USTTask_SheatheWeapon::CompleteSheathe()
{
    if (!Enemy) return;

    // Set weapon tags
    if (UDBAIStateComponent* TagState = Enemy->FindComponentByClass<UDBAIStateComponent>())
    {
        TagState->RemoveWeapon_Drawn();
        TagState->SetWeapon_Sheathed();
    }

    // TODO: Hide weapon mesh
    // TODO: Attach weapon to sheathe socket
}