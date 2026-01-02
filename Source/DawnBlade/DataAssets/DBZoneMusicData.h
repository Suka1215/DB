// DBZoneMusicData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DBZoneMusicData.generated.h"

UCLASS(BlueprintType)
class DAWNBLADE_API UDBZoneMusicData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	FName ZoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	USoundBase* ExplorationMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	USoundBase* BattleMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music|Optional")
	USoundBase* NightMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music|Optional")
	USoundBase* BossBattleMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	float FadeInTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	float FadeOutTime = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone Music")
	bool bLoopMusic = true;
};