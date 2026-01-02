// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/ANS_SheathWeapon.h"

#include "Components/DBEquipmentComponent.h"
#include "Components/DBWeaponComponent.h"
#include "GameFramework/Character.h"

void UANS_SheathWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UDBWeaponComponent* WeaponComp = Character->FindComponentByClass<UDBWeaponComponent>();
	if (!WeaponComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS_SheathWeapon: No Weapon Component found"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("🎬 ANS_SheathWeapon: Moving weapons to sheathed sockets"));
}

void UANS_SheathWeapon::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    
	// Don't do anything on tick - let NotifyEnd handle it
}

void UANS_SheathWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UDBWeaponComponent* WeaponComp = Character->FindComponentByClass<UDBWeaponComponent>();
	if (!WeaponComp)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("🎬 ANS_SheathWeapon: Weapon sheath complete - weapons moved"));

	// Use the weapon component's helper methods to move weapons
	WeaponComp->MoveWeaponsToSheathedSockets();
}
