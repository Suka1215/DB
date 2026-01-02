// DBMusicManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DataAssets/DBZoneMusicData.h"
#include "Abilities/GameplayAbilityTypes.h"  // Add this for FGameplayEventData
#include "DBMusicManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EMusicState : uint8
{
	Exploration,
	Battle,
	BossBattle,
	Night
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBMusicManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDBMusicManagerComponent();

	// Set current zone music data
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetZoneMusic(UDBZoneMusicData* NewZoneMusic);

	// Force a specific music state (for boss battles, etc.)
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetMusicState(EMusicState NewState);

	UPROPERTY(EditDefaultsOnly, Category = "Music|Fallback")
	USoundBase* DefaultExplorationMusic;

	UPROPERTY(EditDefaultsOnly, Category = "Music|Fallback")
	USoundBase* DefaultBattleMusic;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAudioComponent* MusicAudioComponent;

	UPROPERTY()
	UDBZoneMusicData* CurrentZoneMusic;

	EMusicState CurrentMusicState;
	bool bInCombat = false;

	// ✅ Changed from pointer to const reference
	void OnWeaponDrawn(const FGameplayEventData* Payload);
	void OnWeaponSheathed(const FGameplayEventData* Payload);

	void TransitionToMusic(USoundBase* NewMusic);
	USoundBase* GetMusicForState(EMusicState State);
};