// DBMenuGameMode.cpp
#include "Player/DBMenuGameMode.h"
#include "GUI/Menus/DBW_MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ADBMenuGameMode::ADBMenuGameMode()
{
    // No pawn needed for menu
    DefaultPawnClass = nullptr;
    PlayerControllerClass = APlayerController::StaticClass();
    HUDClass = nullptr;
    PlayerStateClass = nullptr;

    // Remove all the ConstructorHelpers code - we'll set this manually
    MainMenuWidgetClass = nullptr;
}

void ADBMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Menu GameMode: BeginPlay started"));

    // Small delay to ensure everything is initialized
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ADBMenuGameMode::CreateMainMenuWidget, 0.1f, false);
}

void ADBMenuGameMode::CreateMainMenuWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Menu GameMode: Creating main menu widget"));

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("Menu GameMode: No PlayerController found!"));
        return;
    }

    if (!MainMenuWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Menu GameMode: MainMenuWidgetClass is not set!"));
        return;
    }

    MainMenuWidget = CreateWidget<UDBW_MainMenu>(PC, MainMenuWidgetClass);
    if (MainMenuWidget)
    {
        MainMenuWidget->AddToViewport();

        // Set input mode to UI only
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PC->SetInputMode(InputModeData);
        PC->SetShowMouseCursor(true);

        UE_LOG(LogTemp, Warning, TEXT("Menu GameMode: Main menu widget created successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Menu GameMode: Failed to create main menu widget"));
    }
}