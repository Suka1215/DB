// ========================================
// UTransitionManager.h - Global Transition Manager
// ========================================
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "TransitionManager.generated.h"

class UUserWidget;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionComplete);

UENUM(BlueprintType)
enum class ETransitionModifier : uint8
{
    Fade,
    Slide,
    Dissolve,
    Custom
};

USTRUCT(BlueprintType)
struct FTransitionSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETransitionModifier TransitionType = ETransitionModifier::Fade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FadeOutDuration = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FadeInDuration = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HoldDuration = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrossfadeAudio = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AudioFadeDuration = 1.0f;
};

UCLASS()
class DAWNBLADE_API UTransitionManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Primary transition function
    UFUNCTION(BlueprintCallable, Category = "Transitions")
    void TransitionToLevel(const FString& LevelName, const FTransitionSettings& Settings = FTransitionSettings());

    // Audio management
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayBackgroundMusic(USoundBase* Music, float FadeInTime = 2.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopBackgroundMusic(float FadeOutTime = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void CrossfadeToMusic(USoundBase* NewMusic, float CrossfadeDuration = 2.0f);

    // Delegate for external systems
    UPROPERTY(BlueprintAssignable)
    FOnTransitionComplete OnTransitionComplete;

protected:
    // Transition overlay widget class
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> TransitionOverlayClass;

private:
    // Current transition overlay
    UPROPERTY()
    UUserWidget* TransitionOverlay;

    // Audio management
    UPROPERTY()
    class UAudioComponent* CurrentMusicComponent;

    UPROPERTY()
    class UAudioComponent* CrossfadeMusicComponent;

    // Transition state
    bool bIsTransitioning = false;
    FString PendingLevelName;
    FTransitionSettings CurrentSettings;

    // Internal methods
    void CreateTransitionOverlay();
    void StartFadeOut();
    void ExecuteLevelTransition();
    void StartFadeIn();
    void CompleteTransition();

    // Audio helpers
    void FadeAudioComponent(UAudioComponent* Component, float TargetVolume, float Duration);
    void CleanupAudioComponent(UAudioComponent* Component);

    // Timer handles
    FTimerHandle FadeOutTimer;
    FTimerHandle TransitionTimer;
    FTimerHandle FadeInTimer;
};