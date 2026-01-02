// ANS_DrawShield.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_DrawShield.generated.h"

/**
 * Anim Notify State for drawing shield
 * Moves shield from sheathed socket to equipped socket
 */
UCLASS()
class DAWNBLADE_API UANS_DrawShield : public UAnimNotifyState
{
	GENERATED_BODY()
    
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override
	{
		return TEXT("Draw Shield");
	}
};