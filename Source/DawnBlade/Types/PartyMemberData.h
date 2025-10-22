#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "EDBJob.h" // your enum for Warrior, LightMage, Paladin
#include "PartyMemberData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FPartyMemberData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDBJob Job = EDBJob::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentMP = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxMP = 50.f;

	// class/job icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* JobIcon = nullptr;

	// active buff icons
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> ActiveBuffs;

	// flag to skip rendering the local player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocalPlayer = false;
};
