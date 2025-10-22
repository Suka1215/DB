// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCharacterData.h"
#include "FParty.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FParty
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PartyId;

	UPROPERTY(BlueprintReadOnly)
	FString PartyName;

	UPROPERTY(BlueprintReadOnly)
	FString LeaderId;

	UPROPERTY(BlueprintReadOnly)
	TArray<FCharacterData> Members;
};
