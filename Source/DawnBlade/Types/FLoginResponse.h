// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FLoginResponse.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FLoginResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Token;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;

	// Optional: username/email returned by API
	UPROPERTY(BlueprintReadOnly)
	FString Username;
};
