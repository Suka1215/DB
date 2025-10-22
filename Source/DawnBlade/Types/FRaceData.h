// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FRaceData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FRaceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 RaceId;

	UPROPERTY(BlueprintReadOnly)
	FString RaceName;

	UPROPERTY(BlueprintReadOnly)
	FString RaceCode;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	FRaceData()
	{
		RaceId = 0;
		RaceName = TEXT("");
		RaceCode = TEXT("");
		Description = TEXT("");
	}
};