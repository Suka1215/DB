// DBAnimInstance.cpp

#include "Animations/DBAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
// REMOVED: #include "KismetAnimationLibrary.h"

void UDBAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
    
    if (OwningCharacter)
    {
        // Get ASC from character
        if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningCharacter))
        {
            AbilitySystemComponent = ASI->GetAbilitySystemComponent();
        }
    }
}

void UDBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwningCharacter || !AbilitySystemComponent) return;

    // Update all animation state from GAS tags and movement
    UpdateCombatState();
    UpdateWeaponState();
    UpdateMovementValues();
}

void UDBAnimInstance::UpdateCombatState()
{
    // Read combat state tags from GAS
    FGameplayTag EngagedTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.Engaged"));
    bIsInCombat = AbilitySystemComponent->HasMatchingGameplayTag(EngagedTag);

    FGameplayTag AutoAttackTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.AutoAttacking"));
    bIsAutoAttacking = AbilitySystemComponent->HasMatchingGameplayTag(AutoAttackTag);

    FGameplayTag CastingTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.Casting"));
    bIsCasting = AbilitySystemComponent->HasMatchingGameplayTag(CastingTag);

    FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
    bIsDead = AbilitySystemComponent->HasMatchingGameplayTag(DeadTag);
}

void UDBAnimInstance::UpdateWeaponState()
{
    // Read weapon/job tags from GAS
    FGameplayTag StaffTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.Staff"));
    bHasStaffEquipped = AbilitySystemComponent->HasMatchingGameplayTag(StaffTag);

    FGameplayTag GreatSwordTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.GreatSword"));
    bHasGreatSwordEquipped = AbilitySystemComponent->HasMatchingGameplayTag(GreatSwordTag);

    FGameplayTag SwordShieldTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.SwordShield"));
    bHasSwordShieldEquipped = AbilitySystemComponent->HasMatchingGameplayTag(SwordShieldTag);
}

void UDBAnimInstance::UpdateMovementValues()
{
    if (UCharacterMovementComponent* MovementComp = OwningCharacter->GetCharacterMovement())
    {
        // Get velocity and speed
        FVector Velocity = MovementComp->Velocity;
        Speed = Velocity.Size();
        
        // FIXED: Calculate direction manually instead of using KismetAnimationLibrary
        if (Speed > 3.f)
        {
            FRotator Rotation = OwningCharacter->GetActorRotation();
            FRotator VelocityRotation = Velocity.Rotation();
            
            // Calculate the difference and normalize to -180 to 180
            Direction = FMath::FindDeltaAngleDegrees(Rotation.Yaw, VelocityRotation.Yaw);
        }
        else
        {
            Direction = 0.f;
        }

        // Check if in air
        bIsInAir = MovementComp->IsFalling();

        // Check if moving (threshold to avoid jitter)
        bIsMoving = Speed > 3.f;
    }
}