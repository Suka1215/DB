// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FJobData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FJobData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 JobId;

	UPROPERTY(BlueprintReadOnly)
	FString JobName;

	UPROPERTY(BlueprintReadOnly)
	FString JobCode;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	FJobData()
	{
		JobId = 0;
		JobName = TEXT("");
		JobCode = TEXT("");
		Description = TEXT("");
	}
};
