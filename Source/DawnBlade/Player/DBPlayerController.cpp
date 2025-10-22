#include "Player/DBPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GUI/HUD/DBW_GameHUD.h"
#include "GUI/Debug/UDBW_AbilityDebugPanel.h"
#include "Characters/DBPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADBPlayerController::ADBPlayerController()
{
    // Find your widgets once at load time
    {
       static ConstructorHelpers::FClassFinder<UUserWidget> HudBP(
          TEXT("/Game/Game/GUI/WBP_GameHUD"));
       if (HudBP.Succeeded()) { GameHUDClass = HudBP.Class; }
    }

    // Mouse cursor hidden for third-person gameplay
    bShowMouseCursor = false;
}

void ADBPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (!IsLocalController()) return;

    CreateOrShowHUD();

    // IMPORTANT: Set to Game Only mode for camera control
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    
    // Hide cursor during gameplay
    bShowMouseCursor = false;

    // If pawn already exists, initialize widgets with it
    InitWidgetsForPawn(Cast<ADBPlayerCharacter>(GetPawn()));
}

void ADBPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    InitWidgetsForPawn(Cast<ADBPlayerCharacter>(InPawn));
}

void ADBPlayerController::CreateOrShowHUD()
{
    if (GameHUDClass && !GameHUDInstance)
    {
       GameHUDInstance = CreateWidget<UUserWidget>(this, GameHUDClass);
       if (GameHUDInstance) 
       { 
           GameHUDInstance->AddToViewport(50);
       }
       else                 
       { 
           UE_LOG(LogTemp, Error, TEXT("CreateWidget failed for GameHUDClass.")); 
       }
    }
    else if (!GameHUDClass)
    {
       UE_LOG(LogTemp, Warning, TEXT("GameHUDClass is not set on DBPlayerController."));
    }

    if (DebugPanelClass && !DebugPanelInstance)
    {
       DebugPanelInstance = CreateWidget<UUserWidget>(this, DebugPanelClass);
       if (DebugPanelInstance) 
       { 
           DebugPanelInstance->AddToViewport(100);
       }
       else                    
       { 
           UE_LOG(LogTemp, Error, TEXT("CreateWidget failed for DebugPanelClass.")); 
       }
    }
}

void ADBPlayerController::InitWidgetsForPawn(ADBPlayerCharacter* PC)
{
    if (!PC) return;

    if (UDBW_GameHUD* HUD = Cast<UDBW_GameHUD>(GameHUDInstance))
    {
       HUD->InitForCharacter(PC);
    }

    if (UDBW_AbilityDebugPanel* Panel = Cast<UDBW_AbilityDebugPanel>(DebugPanelInstance))
    {
       Panel->InitForCharacter(PC);
    }
}