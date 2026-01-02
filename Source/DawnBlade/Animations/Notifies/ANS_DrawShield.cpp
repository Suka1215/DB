// ANS_DrawShield.cpp
#include "Animations/Notifies/ANS_DrawShield.h"
#include "Components/DBWeaponComponent.h"
#include "Components/DBEquipmentComponent.h"
#include "GameFramework/Character.h"

void UANS_DrawShield::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
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
		UE_LOG(LogTemp, Warning, TEXT("ANS_DrawShield: No Weapon Component found"));
		return;
	}

	// Get weapon data to find shield socket
	UDBWeaponAnimationSet* WeaponData = WeaponComp->GetCurrentWeaponData();
	if (!WeaponData || !WeaponData->bUsesOffHand)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS_DrawShield: Current weapon doesn't use off-hand"));
		return;
	}

	UDBEquipmentComponent* EquipComp = Character->FindComponentByClass<UDBEquipmentComponent>();
	if (!EquipComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS_DrawShield: No Equipment Component found"));
		return;
	}

	// Move shield from back to hand
	EquipComp->MoveEquipmentToSocket(
		EEquipmentSlot::OffHand,
		WeaponData->OffHandDrawnSocket
	);

	UE_LOG(LogTemp, Warning, TEXT("🛡️ ANS_DrawShield: Moving shield to hand"));
}

void UANS_DrawShield::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UE_LOG(LogTemp, Warning, TEXT("🛡️ ANS_DrawShield: Shield draw complete"));
    
	// Shield is now ready - could add additional logic here if needed
	// For example, play a "shield ready" sound effect
}