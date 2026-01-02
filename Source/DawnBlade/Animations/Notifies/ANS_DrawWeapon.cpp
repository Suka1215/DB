// ANS_DrawWeapon.cpp
#include "Animations/Notifies/ANS_DrawWeapon.h"

#include "Components/DBEquipmentComponent.h"
#include "Components/DBWeaponComponent.h"
#include "GameFramework/Character.h"

void UANS_DrawWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    
	// Get socket names once at the start
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

	UE_LOG(LogTemp, Warning, TEXT("🎬 ANS_DrawWeapon: Starting weapon draw"));
}

void UANS_DrawWeapon::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    
	// Don't do anything on tick - let NotifyEnd handle it
}

void UANS_DrawWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	UE_LOG(LogTemp, Warning, TEXT("🎬 ANS_DrawWeapon: Weapon draw complete - moving to drawn sockets"));

	// Move weapons ONCE at the end
	WeaponComp->MoveWeaponsToDrawnSockets();
}