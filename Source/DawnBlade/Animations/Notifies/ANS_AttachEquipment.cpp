// ANS_AttachEquipment.cpp - CORRECTED VERSION
#include "Animations/Notifies/ANS_AttachEquipment.h"
#include "DawnBlade/Components/DBEquipmentComponent.h"
#include "GameFramework/Character.h"

void UANS_AttachEquipment::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("🔔 ANS_AttachEquipment NOTIFY FIRED!"));
	UE_LOG(LogTemp, Warning, TEXT("   Slot Tag: %s"), *EquipmentSlotTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("   State Tag: %s"), *AttachmentStateTag.ToString());
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UDBEquipmentComponent* EquipmentComp = Character->FindComponentByClass<UDBEquipmentComponent>();
	if (!EquipmentComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS_AttachEquipment: No Equipment Component found on character"));
		return;
	}

	// Validate tags
	if (!EquipmentSlotTag.IsValid() || !AttachmentStateTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ANS_AttachEquipment: Invalid tags! Slot:%s State:%s"), 
			*EquipmentSlotTag.ToString(), *AttachmentStateTag.ToString());
		return;
	}

	// Call the data-driven attachment method
	EquipmentComp->AttachEquipmentByTags(EquipmentSlotTag, AttachmentStateTag);
}