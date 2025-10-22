// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString AuthToken;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerId; // fill after login
};
