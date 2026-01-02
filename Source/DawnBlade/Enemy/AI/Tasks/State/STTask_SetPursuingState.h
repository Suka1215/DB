// Copyright DawnBlade. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SetPursuingState.generated.h"

/**
 * State Tree Task: Set Pursuing State
 * 
 * Sets State.AI.Pursuing tag when enemy is chasing a lost target.
 * Used when target moves out of range but enemy hasn't given up yet.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Set Pursuing State"))
class DAWNBLADE_API USTTask_SetPursuingState : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_SetPursuingState(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
    /** Enemy character reference */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<ACharacter> Enemy;
};