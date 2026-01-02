// ANS_DrawWeapon.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_DrawWeapon.generated.h"

/**
 * Anim Notify State for drawing weapons
 * Moves weapons from sheathed sockets to equipped sockets
 */
UCLASS()
class DAWNBLADE_API UANS_DrawWeapon : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    virtual FString GetNotifyName_Implementation() const override
    {
        return TEXT("Draw Weapon");
    }

protected:
    // Which slots to move (MainHand, OffHand, or both)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bMoveMainHand = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bMoveOffHand = true;
};