// DBGameMode.cpp
#include "Player/DBGameMode.h"
#include "Characters/DBPlayerCharacter.h"
#include "Player/DBPlayerController.h"
#include "Player/DBPlayerState.h"
#include "Player/DBHUD.h"
#include "Account/DBAuthSubsystem.h"
#include "Engine/GameInstance.h"

ADBGameMode::ADBGameMode()
{
	// Set all your custom gameplay classes
	DefaultPawnClass = ADBPlayerCharacter::StaticClass();
	PlayerControllerClass = ADBPlayerController::StaticClass();
	PlayerStateClass = ADBPlayerState::StaticClass();
	HUDClass = ADBHUD::StaticClass();
}

void ADBGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Gameplay GameMode: BeginPlay started"));

	// Optional: Verify user is logged in when entering gameplay
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
		{
			if (!AuthSubsystem->IsLoggedIn())
			{
				UE_LOG(LogTemp, Warning, TEXT("User not logged in, redirecting to menu..."));
                
				// Optional: Redirect back to menu if not logged in
				// UGameplayStatics::OpenLevel(this, TEXT("MenuLevel"));
				return;
			}
			else
			{
				FPlayerData PlayerData = AuthSubsystem->GetPlayerData();
				UE_LOG(LogTemp, Warning, TEXT("Gameplay started for user: %s"), *PlayerData.Username);
                
				// Optional: Load user's character data here
				// AuthSubsystem->LoadCharacters();
			}
		}
	}

	// Any other gameplay initialization here
}