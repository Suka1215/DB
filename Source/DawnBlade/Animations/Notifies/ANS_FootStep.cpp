// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_FootStep.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

void UANS_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !FootstepSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("👟 Footstep notify triggered but missing MeshComp or Sound"));
		return;
	}

	// Get the socket location (foot position)
	FVector SocketLocation = MeshComp->GetSocketLocation(SocketName);

	// Add slight random pitch variation for more natural sound
	float RandomPitch = FMath::RandRange(0.95f, 1.05f) * PitchMultiplier;

	// ✅ Simplified - UE 5.7 version
	UGameplayStatics::PlaySoundAtLocation(
		MeshComp->GetWorld(),
		FootstepSound,
		SocketLocation,
		VolumeMultiplier,
		RandomPitch
	);

	UE_LOG(LogTemp, Verbose, TEXT("👟 Footstep played at %s"), *SocketLocation.ToString());
}

FString UANS_FootStep::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Footstep: %s"), *SocketName.ToString());
}