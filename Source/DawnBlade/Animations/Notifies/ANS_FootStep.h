// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ANS_FootStep.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UANS_FootStep : public UAnimNotify
{
	GENERATED_BODY()

public:
	// The sound to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	USoundBase* FootstepSound;

	// Which foot socket to use for positioning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	FName SocketName = TEXT("foot_r");

	// Volume multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float VolumeMultiplier = 1.0f;

	// Pitch variation range (randomization)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float PitchMultiplier = 1.0f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
