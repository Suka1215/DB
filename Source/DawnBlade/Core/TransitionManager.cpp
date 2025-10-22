// ========================================
// UTransitionManager.cpp
// ========================================
#include "Core/TransitionManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "AudioDevice.h"

void UTransitionManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Don't load the class here - we'll set it via Blueprint or load it dynamically
    UE_LOG(LogTemp, Log, TEXT("TransitionManager initialized"));
}

void UTransitionManager::TransitionToLevel(const FString& LevelName, const FTransitionSettings& Settings)
{
    if (bIsTransitioning)
    {
        UE_LOG(LogTemp, Warning, TEXT("Transition already in progress, ignoring new request"));
        return;
    }

    bIsTransitioning = true;
    PendingLevelName = LevelName;
    CurrentSettings = Settings;

    UE_LOG(LogTemp, Log, TEXT("Beginning transition to level: %s"), *LevelName);

    // Create or reuse transition overlay
    CreateTransitionOverlay();

    // Start the transition sequence
    StartFadeOut();
}

void UTransitionManager::CreateTransitionOverlay()
{
    if (!TransitionOverlay)
    {
        if (!TransitionOverlayClass)
        {
            // Updated path to match your widget location
            UClass* LoadedClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Game/GUI/WBP_TransitionOverlay.WBP_TransitionOverlay_C"));
            if (LoadedClass)
            {
                TransitionOverlayClass = LoadedClass;
                UE_LOG(LogTemp, Warning, TEXT("Successfully loaded transition overlay class"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load transition overlay class at path: /Game/Game/GUI/WBP_TransitionOverlay.WBP_TransitionOverlay_C"));
                return;
            }
        }

        if (UWorld* World = GetWorld())
        {
            if (APlayerController* PC = World->GetFirstPlayerController())
            {
                TransitionOverlay = CreateWidget<UUserWidget>(PC, TransitionOverlayClass);
                if (TransitionOverlay)
                {
                    TransitionOverlay->AddToViewport(10000);
                    TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
                    UE_LOG(LogTemp, Warning, TEXT("Transition overlay created successfully"));
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to create transition overlay widget"));
                }
            }
        }
    }
}

void UTransitionManager::StartFadeOut()
{
    if (!TransitionOverlay)
    {
        UE_LOG(LogTemp, Error, TEXT("No transition overlay available"));
        CompleteTransition();
        return;
    }

    // Make overlay visible and start fade out animation
    TransitionOverlay->SetVisibility(ESlateVisibility::Visible);

    // Call Blueprint function to start fade out animation
    if (UFunction* FadeOutFunc = TransitionOverlay->FindFunction(FName("StartFadeOut")))
    {
        TransitionOverlay->ProcessEvent(FadeOutFunc, nullptr);
    }

    // Start audio fade if enabled
    if (CurrentSettings.bCrossfadeAudio && CurrentMusicComponent)
    {
        FadeAudioComponent(CurrentMusicComponent, 0.0f, CurrentSettings.AudioFadeDuration);
    }

    // Set timer for transition execution
    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        this,
        &UTransitionManager::ExecuteLevelTransition,
        CurrentSettings.FadeOutDuration + CurrentSettings.HoldDuration,
        false
    );
}

void UTransitionManager::ExecuteLevelTransition()
{
    UE_LOG(LogTemp, Log, TEXT("Executing level transition to: %s"), *PendingLevelName);

    // Clean up current audio
    if (CurrentMusicComponent)
    {
        CleanupAudioComponent(CurrentMusicComponent);
    }

    // Execute the level change
    UGameplayStatics::OpenLevel(this, FName(*PendingLevelName));

    // Set timer for fade in (will execute in new level)
    GetWorld()->GetTimerManager().SetTimer(
        FadeInTimer,
        this,
        &UTransitionManager::StartFadeIn,
        0.1f, // Small delay to ensure new level is loaded
        false
    );
}

void UTransitionManager::StartFadeIn()
{
    // Recreate overlay in new level if needed
    if (!TransitionOverlay)
    {
        CreateTransitionOverlay();
    }

    if (TransitionOverlay)
    {
        // Call Blueprint function to start fade in animation
        if (UFunction* FadeInFunc = TransitionOverlay->FindFunction(FName("StartFadeIn")))
        {
            TransitionOverlay->ProcessEvent(FadeInFunc, nullptr);
        }

        // Set timer to complete transition
        GetWorld()->GetTimerManager().SetTimer(
            FadeInTimer,
            this,
            &UTransitionManager::CompleteTransition,
            CurrentSettings.FadeInDuration,
            false
        );
    }
    else
    {
        CompleteTransition();
    }
}

void UTransitionManager::CompleteTransition()
{
    UE_LOG(LogTemp, Log, TEXT("Transition completed"));

    // Hide transition overlay
    if (TransitionOverlay)
    {
        TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Reset state
    bIsTransitioning = false;
    PendingLevelName.Empty();

    // Broadcast completion
    OnTransitionComplete.Broadcast();
}

void UTransitionManager::PlayBackgroundMusic(USoundBase* Music, float FadeInTime)
{
    if (!Music) return;

    // Stop existing music
    if (CurrentMusicComponent)
    {
        StopBackgroundMusic(0.5f);
    }

    // Create new audio component
    CurrentMusicComponent = UGameplayStatics::CreateSound2D(
        this,
        Music,
        0.0f, // Start at 0 volume
        1.0f,
        0.0f,
        nullptr,
        true,  // Persist across level transitions
        false
    );

    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->Play();
        
        // Fade in
        FadeAudioComponent(CurrentMusicComponent, 0.7f, FadeInTime);
        
        UE_LOG(LogTemp, Log, TEXT("Started background music with fade in"));
    }
}

void UTransitionManager::StopBackgroundMusic(float FadeOutTime)
{
    if (CurrentMusicComponent)
    {
        FadeAudioComponent(CurrentMusicComponent, 0.0f, FadeOutTime);
        
        // Clean up after fade completes
        FTimerHandle CleanupTimer;
        GetWorld()->GetTimerManager().SetTimer(
            CleanupTimer,
            [this]() { CleanupAudioComponent(CurrentMusicComponent); },
            FadeOutTime + 0.1f,
            false
        );
    }
}

void UTransitionManager::CrossfadeToMusic(USoundBase* NewMusic, float CrossfadeDuration)
{
    if (!NewMusic) return;

    // Store reference to old component for fade out
    UAudioComponent* OldComponent = CurrentMusicComponent;

    // Create new audio component
    CurrentMusicComponent = UGameplayStatics::CreateSound2D(
        this,
        NewMusic,
        0.0f,
        1.0f,
        0.0f,
        nullptr,
        true,
        false
    );

    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->Play();
        
        // Fade in new music
        FadeAudioComponent(CurrentMusicComponent, 0.7f, CrossfadeDuration);
        
        // Fade out old music
        if (OldComponent)
        {
            FadeAudioComponent(OldComponent, 0.0f, CrossfadeDuration);
            
            // Clean up old component after fade
            FTimerHandle CleanupTimer;
            GetWorld()->GetTimerManager().SetTimer(
                CleanupTimer,
                [this, OldComponent]() { CleanupAudioComponent(OldComponent); },
                CrossfadeDuration + 0.1f,
                false
            );
        }
    }
}

void UTransitionManager::FadeAudioComponent(UAudioComponent* Component, float TargetVolume, float Duration)
{
    if (!Component) return;

    Component->FadeOut(Duration, TargetVolume);
}

void UTransitionManager::CleanupAudioComponent(UAudioComponent* Component)
{
    if (Component)
    {
        Component->Stop();
        Component = nullptr;
    }
}