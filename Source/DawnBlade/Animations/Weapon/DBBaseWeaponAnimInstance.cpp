// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Weapon/DBBaseWeaponAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DBWeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UDBBaseWeaponAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwningCharacter)
    {
        WeaponComponent = OwningCharacter->FindComponentByClass<UDBWeaponComponent>();
    }

    UE_LOG(LogTemp, Warning, TEXT("🎬 Anim Instance initialized - Weapon Component: %s"), 
        WeaponComponent ? TEXT("Found") : TEXT("NOT FOUND"));
}

void UDBBaseWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwningCharacter) return;

    // Update weapon data
    if (WeaponComponent)
    {
        CurrentWeaponData = WeaponComponent->GetCurrentWeaponData();
        bWeaponDrawn = WeaponComponent->IsWeaponDrawn();
    }

    // Update movement
    const FVector Velocity = OwningCharacter->GetVelocity();
    Speed = Velocity.Size2D();

    // Get direction for strafing
    if (Speed > 0.0f)
    {
        const FRotator CharacterRotation = OwningCharacter->GetActorRotation();
        Direction = UKismetMathLibrary::NormalizedDeltaRotator(
            Velocity.Rotation(), 
            CharacterRotation
        ).Yaw;
    }
    else
    {
        Direction = 0.0f;
    }

    // Check falling
    if (UCharacterMovementComponent* Movement = OwningCharacter->GetCharacterMovement())
    {
        bIsFalling = Movement->IsFalling();
    }
}

UBlendSpace* UDBBaseWeaponAnimInstance::GetLocomotionBlendspace() const
{
    if (!CurrentWeaponData)
    {
        return nullptr;
    }

    // If weapon is drawn, use combat blendspace
    if (bWeaponDrawn && CurrentWeaponData->CombatLocomotionBlendspace)
    {
        return CurrentWeaponData->CombatLocomotionBlendspace;
    }

    // Otherwise use parent/unarmed (return null to fall back to parent ABP)
    return nullptr;
}
