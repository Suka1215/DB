// Fill out your copyright notice in the Description page of Project Settings.


// ========================================
// DBTitleScreenWidget.cpp
// ========================================
#include "GUI/Menus/DBW_TitleScreen.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/TransitionManager.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UDBW_TitleScreen::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button events
    if (StartButton)
        StartButton->OnClicked.AddDynamic(this, &UDBW_TitleScreen::OnStartClicked);
    
    if (OptionsButton)
        OptionsButton->OnClicked.AddDynamic(this, &UDBW_TitleScreen::OnOptionsClicked);
    
    if (CreditsButton)
        CreditsButton->OnClicked.AddDynamic(this, &UDBW_TitleScreen::OnCreditsClicked);
    
    if (QuitButton)
        QuitButton->OnClicked.AddDynamic(this, &UDBW_TitleScreen::OnQuitClicked);

    // Set version text
    if (versionText)
    {
        FString GameVersion = TEXT("Version 1.2.3b - Build 20241119");
        versionText->SetText(FText::FromString(GameVersion));
    }

    // Start background music
    StartBackgroundMusic();

    // Configure transition settings
    TransitionSettings.TransitionType = ETransitionModifier::Fade;
    TransitionSettings.FadeOutDuration = 1.2f;
    TransitionSettings.FadeInDuration = 0.8f;
    TransitionSettings.HoldDuration = 0.3f;
    TransitionSettings.bCrossfadeAudio = true;
    TransitionSettings.AudioFadeDuration = 1.5f;
}

void UDBW_TitleScreen::NativeDestruct()
{
    Super::NativeDestruct();
}

void UDBW_TitleScreen::OnStartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Start button clicked - beginning smooth transition"));
    
    // Disable buttons during transition
    if (StartButton) StartButton->SetIsEnabled(false);
    if (OptionsButton) OptionsButton->SetIsEnabled(false);
    if (CreditsButton) CreditsButton->SetIsEnabled(false);
    if (QuitButton) QuitButton->SetIsEnabled(false);

    // Use transition manager for smooth transition
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UTransitionManager* TransitionManager = GI->GetSubsystem<UTransitionManager>())
        {
            TransitionManager->TransitionToLevel(TEXT("LoginLevel"), TransitionSettings);
        }
    }
}

void UDBW_TitleScreen::OnOptionsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Options clicked"));
    // TODO: Implement options screen with transition
}

void UDBW_TitleScreen::OnCreditsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Credits clicked"));
    // TODO: Implement credits screen with transition
}

void UDBW_TitleScreen::OnQuitClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Quit clicked"));
    
    // Disable buttons during quit sequence
    if (StartButton) StartButton->SetIsEnabled(false);
    if (OptionsButton) OptionsButton->SetIsEnabled(false);
    if (CreditsButton) CreditsButton->SetIsEnabled(false);
    if (QuitButton) QuitButton->SetIsEnabled(false);
    
    // Stop background music with fade
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UTransitionManager* TransitionManager = GI->GetSubsystem<UTransitionManager>())
        {
            TransitionManager->StopBackgroundMusic(1.0f);
        }
    }
    
    // Delayed quit after music fades
    FTimerHandle QuitTimer;
    GetWorld()->GetTimerManager().SetTimer(QuitTimer, []()
    {
        UKismetSystemLibrary::QuitGame(nullptr, nullptr, EQuitPreference::Quit, false);
    }, 1.0f, false);
}

void UDBW_TitleScreen::StartBackgroundMusic()
{
    if (TitleMusic)
    {
        // Use the simple, direct approach that bypasses the complex audio component system
        UGameplayStatics::PlaySound2D(this, TitleMusic, 0.7f, 1.0f, 0.0f);
        UE_LOG(LogTemp, Warning, TEXT("Playing title music directly"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TitleMusic asset not set"));
    }
}