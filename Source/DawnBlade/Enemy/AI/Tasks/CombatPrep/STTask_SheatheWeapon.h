// Copyright DawnBlade. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_SheatheWeapon.generated.h"

/**
 * State Tree Task: Sheathe Weapon
 * 
 * Plays sheathe animation and sets Weapon.Sheathed tag.
 * Opposite of STTask_DrawWeapon.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Sheathe Weapon"))
class DAWNBLADE_API USTTask_SheatheWeapon : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_SheatheWeapon(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
    /** Enemy character reference */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<ACharacter> Enemy;

    /** Sheathe animation montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<UAnimMontage> SheatheMontage;

    /** Sheathe time (if no montage) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float SheatheTime = 1.0f;

    /** Sheathe successful */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bSheatheSuccessful = false;

private:
    float ElapsedTime = 0.0f;
    bool bIsSheathing = false;

    // Native gameplay tags
    static FGameplayTag Tag_State_Weapon_Drawn;
    static FGameplayTag Tag_State_Weapon_Sheathed;

    void StartSheathe();
    void CompleteSheathe();
};