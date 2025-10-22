// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterRaceSelectGameMode.h"
#include "GUI/CharacterRaceLineup.h"
#include "GUI/RaceSelectWidget.h"
#include "Blueprint/UserWidget.h"

ACharacterRaceSelectGameMode::ACharacterRaceSelectGameMode()
{
	// You can set defaults here if needed, e.g. default pawn, HUD, etc.
	// DefaultPawnClass = AYourPawn::StaticClass();
	// HUDClass = AYourHUD::StaticClass();
}

void ACharacterRaceSelectGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Params;
	ACharacterRaceLineup* Lineup = GetWorld()->SpawnActor<ACharacterRaceLineup>(
		LineupClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	URaceSelectWidget* UI = CreateWidget<URaceSelectWidget>(GetWorld(), RaceSelectWidgetClass);
	if (UI)
	{
		UI->LineupActor = Lineup;
		UI->AddToViewport();
	}
}
