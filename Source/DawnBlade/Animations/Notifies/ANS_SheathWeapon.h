// ANS_SheathWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_SheathWeapon.generated.h"

/**
 * Anim Notify State for sheathing weapons
 * Moves weapons from equipped sockets back to sheathed sockets
 */
UCLASS()
class DAWNBLADE_API UANS_SheathWeapon : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override
	{
		return TEXT("Sheath Weapon");
	}

protected:
	// Which slots to move (MainHand, OffHand, or both)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bMoveMainHand = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bMoveOffHand = true;
};