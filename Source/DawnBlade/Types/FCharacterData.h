// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FJobData.h"
#include "FRaceData.h"
#include "FCharacterData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString CharacterId;

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly)
	FJobData Job;

	UPROPERTY(BlueprintReadOnly)
	FRaceData Race;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHP = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxHP = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentMP = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxMP = 0;
};
