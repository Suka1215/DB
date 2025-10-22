// Fill out your copyright notice in the Description page of Project Settings.


// ========================================
// DBTitleGameMode.cpp
// ========================================
#include "Player/DBTitleGameMode.h"
#include "GUI/Menus/DBW_TitleScreen.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

ADBTitleGameMode::ADBTitleGameMode()
{
    // No pawn needed for menu
    DefaultPawnClass = nullptr;
    PlayerControllerClass = APlayerController::StaticClass();
    HUDClass = nullptr;
    PlayerStateClass = nullptr;

    // Initialize widget pointer
    TitleScreenWidget = nullptr;
}

void ADBTitleGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Title GameMode: BeginPlay started"));

    // Small delay to ensure everything is initialized
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ADBTitleGameMode::CreateTitleScreenWidget, 0.1f, false);
}

void ADBTitleGameMode::CreateTitleScreenWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Title GameMode: Creating title screen widget"));

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("Title GameMode: No PlayerController found!"));
        return;
    }

    // Fixed null check for UE5.6 - use proper TSubclassOf checking
    if (!TitleScreenWidgetClass || !IsValid(TitleScreenWidgetClass))
    {
        UE_LOG(LogTemp, Error, TEXT("Title GameMode: TitleScreenWidgetClass is not set!"));
        return;
    }

    // Create the widget using the correct class name
    TitleScreenWidget = CreateWidget<UDBW_TitleScreen>(PC, TitleScreenWidgetClass);
    if (TitleScreenWidget)
    {
        TitleScreenWidget->AddToViewport();
        UE_LOG(LogTemp, Error, TEXT("Widget added to viewport - check if visible on screen"));

        // Set input mode to UI only
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(TitleScreenWidget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PC->SetInputMode(InputModeData);
        PC->SetShowMouseCursor(true);

        UE_LOG(LogTemp, Warning, TEXT("Title GameMode: Title screen widget created successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Title GameMode: Failed to create title screen widget"));
    }
}