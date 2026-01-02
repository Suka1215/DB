#include "DBCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UDBCameraManager::UDBCameraManager()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDBCameraManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Start in top-down mode
    ApplyCameraMode(ECameraMode::TopDown, true);
}

void UDBCameraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsTransitioning)
    {
        TransitionTimer += DeltaTime;
        float Alpha = FMath::Clamp(TransitionTimer / TransitionDuration, 0.0f, 1.0f);
        
        // Smooth ease-in-out
        Alpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);
        
        UpdateCameraTransform(Alpha);

        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
            CurrentMode = TargetMode;
            ApplyCameraMode(CurrentMode, true);
        }
    }
}

USpringArmComponent* UDBCameraManager::GetSpringArm() const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return nullptr;
    
    // Look for your CameraBoom specifically
    return Cast<USpringArmComponent>(Character->GetComponentByClass(USpringArmComponent::StaticClass()));
}

void UDBCameraManager::SetCameraMode(ECameraMode NewMode, float InTransitionDuration)
{
    if (NewMode == CurrentMode && !bIsTransitioning)
        return;

    USpringArmComponent* SpringArm = GetSpringArm();
    if (!SpringArm) return;

    // Store current state as transition start
    TransitionStartLocation = SpringArm->GetRelativeLocation();
    TransitionStartRotation = SpringArm->GetRelativeRotation();
    TransitionStartArmLength = SpringArm->TargetArmLength;
    TransitionStartLagSpeed = SpringArm->CameraLagSpeed;

    // Set up transition
    TargetMode = NewMode;
    TransitionDuration = FMath::Max(InTransitionDuration, 0.1f); // Use the member variable now
    TransitionTimer = 0.0f;
    bIsTransitioning = true;
}

void UDBCameraManager::UpdateCameraTransform(float Alpha)
{
    USpringArmComponent* SpringArm = GetSpringArm();
    if (!SpringArm) return;

    // Determine target values
    FVector TargetLocation;
    FRotator TargetRotation;
    float TargetArmLength;
    float TargetLagSpeed;

    if (TargetMode == ECameraMode::TopDown)
    {
        TargetLocation = FVector(0, 0, TopDownHeight);
        TargetRotation = FRotator(TopDownPitch, 0, 0);
        TargetArmLength = TopDownArmLength;
        TargetLagSpeed = TopDownLagSpeed;
    }
    else // Combat
    {
        TargetLocation = FVector(0, 0, CombatHeight);
        TargetRotation = FRotator(CombatPitch, 0, 0);
        TargetArmLength = CombatArmLength;
        TargetLagSpeed = CombatLagSpeed;
    }

    // Lerp all properties
    FVector NewLocation = FMath::Lerp(TransitionStartLocation, TargetLocation, Alpha);
    FRotator NewRotation = FMath::Lerp(TransitionStartRotation, TargetRotation, Alpha);
    float NewArmLength = FMath::Lerp(TransitionStartArmLength, TargetArmLength, Alpha);
    float NewLagSpeed = FMath::Lerp(TransitionStartLagSpeed, TargetLagSpeed, Alpha);

    SpringArm->SetRelativeLocation(NewLocation);
    SpringArm->SetRelativeRotation(NewRotation);
    SpringArm->TargetArmLength = NewArmLength;
    SpringArm->CameraLagSpeed = NewLagSpeed;
}

void UDBCameraManager::ApplyCameraMode(ECameraMode Mode, bool bInstant)
{
    USpringArmComponent* SpringArm = GetSpringArm();
    if (!SpringArm) return;

    if (Mode == ECameraMode::TopDown)
    {
        SpringArm->SetRelativeLocation(FVector(0, 0, TopDownHeight));
        SpringArm->SetRelativeRotation(FRotator(TopDownPitch, 0, 0));
        SpringArm->TargetArmLength = TopDownArmLength;
        SpringArm->bEnableCameraLag = true;
        SpringArm->CameraLagSpeed = TopDownLagSpeed;
        SpringArm->bUsePawnControlRotation = false; // Don't follow character rotation
    }
    else // Combat
    {
        SpringArm->SetRelativeLocation(FVector(0, 0, CombatHeight));
        SpringArm->SetRelativeRotation(FRotator(CombatPitch, 0, 0));
        SpringArm->TargetArmLength = CombatArmLength;
        SpringArm->bEnableCameraLag = true;
        SpringArm->CameraLagSpeed = CombatLagSpeed;
        SpringArm->bUsePawnControlRotation = false; // Change to true if you want camera to rotate with character in combat
    }
}