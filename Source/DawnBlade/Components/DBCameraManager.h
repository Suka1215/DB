#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DBCameraManager.generated.h"

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
    TopDown     UMETA(DisplayName = "Top-Down Exploration"),
    Combat      UMETA(DisplayName = "Combat View"),
    Transition  UMETA(DisplayName = "Transitioning")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBCameraManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UDBCameraManager();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraMode(ECameraMode NewMode, float InTransitionDuration = 1.0f);

    UFUNCTION(BlueprintPure, Category = "Camera")
    ECameraMode GetCurrentCameraMode() const { return CurrentMode; }

    // Top-Down Camera Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top-Down Camera")
    float TopDownHeight = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top-Down Camera")
    float TopDownPitch = -75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top-Down Camera")
    float TopDownArmLength = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top-Down Camera")
    float TopDownLagSpeed = 3.0f;

    // Combat Camera Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Camera")
    float CombatHeight = 70.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Camera")
    float CombatPitch = -20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Camera")
    float CombatArmLength = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Camera")
    float CombatLagSpeed = 10.0f;

protected:
    virtual void BeginPlay() override;

private:
    ECameraMode CurrentMode = ECameraMode::TopDown;
    ECameraMode TargetMode = ECameraMode::TopDown;

    float TransitionTimer = 0.0f;
    float TransitionDuration = 1.0f;
    bool bIsTransitioning = false;

    FVector TransitionStartLocation;
    FRotator TransitionStartRotation;
    float TransitionStartArmLength;
    float TransitionStartLagSpeed;

    void UpdateCameraTransform(float Alpha);
    void ApplyCameraMode(ECameraMode Mode, bool bInstant = false);
    class USpringArmComponent* GetSpringArm() const;
};