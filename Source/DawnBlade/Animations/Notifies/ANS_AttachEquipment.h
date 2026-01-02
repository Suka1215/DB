// ANS_AttachEquipment.h - CORRECTED VERSION
// This replaces your existing ANS_AttachEquipment files
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "ANS_AttachEquipment.generated.h"

/**
 * Data-driven animation notify for attaching equipment to sockets
 * Uses gameplay tags instead of hardcoded enums for full data-driven flexibility
 */
UCLASS()
class DAWNBLADE_API UANS_AttachEquipment : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// Equipment slot tag - e.g., "Equipment.Slot.MainWeapon" or "Equipment.Slot.Shield"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag EquipmentSlotTag;

	// Attachment state tag - e.g., "Equipment.State.Equipped" or "Equipment.State.Sheathed"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (Categories = "Equipment.State"))
	FGameplayTag AttachmentStateTag;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override
	{
		FString SlotName = EquipmentSlotTag.IsValid() ? EquipmentSlotTag.GetTagName().ToString() : TEXT("None");
		FString StateName = AttachmentStateTag.IsValid() ? AttachmentStateTag.GetTagName().ToString() : TEXT("None");
		return FString::Printf(TEXT("Attach: %s (%s)"), *SlotName, *StateName);
	}
#endif
};