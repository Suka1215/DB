// DBMusicZone.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAssets/DBZoneMusicData.h"
#include "DBMusicZone.generated.h"

UCLASS()
class DAWNBLADE_API ADBMusicZone : public AActor
{
	GENERATED_BODY()

public:
	ADBMusicZone();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	class UBoxComponent* ZoneVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone Music")
	UDBZoneMusicData* ZoneMusicData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	FName ZoneName;

	// For debugging in editor
	UPROPERTY(EditAnywhere, Category = "Zone|Debug")
	bool bShowDebugBounds = true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};