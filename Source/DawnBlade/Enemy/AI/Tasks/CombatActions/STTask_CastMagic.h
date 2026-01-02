// Copyright DawnBlade. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "STTask_CastMagic.generated.h"

/**
 * State Tree Task: Cast Magic
 * 
 * FFXI-style magic casting with MP cost, cast time, and interrupt support.
 * Sets State.AI.Casting tag during cast.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Cast Magic"))
class DAWNBLADE_API USTTask_CastMagic : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_CastMagic(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
    /** Enemy character reference */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<ACharacter> Enemy;

    /** Spell to cast (e.g., Magic.Fire.Fire1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FGameplayTag SpellTag;

    /** MP cost */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    int32 ManaCost = 20;

    /** Cast time in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float CastTime = 2.5f;

    /** Casting animation montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<UAnimMontage> CastMontage;

    /** Target for the spell */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TObjectPtr<AActor> TargetActor;

    /** Face target while casting */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bFaceTargetWhileCasting = true;

    /** Can be interrupted */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bCanBeInterrupted = true;

    /** Cast successful */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bCastSuccessful = false;

    /** Cast interrupted */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
    bool bCastInterrupted = false;

private:
    float ElapsedCastTime = 0.0f;
    bool bIsCasting = false;

    // Native gameplay tags
    static FGameplayTag Tag_State_AI_Casting;
    static FGameplayTag Tag_State_Combat_Casting;

    bool HasSufficientMana() const;
    void ConsumeMana();
    void StartCasting();
    void CompleteCast();
    void InterruptCast();
    int32 GetCurrentMana() const;
    void SetCurrentMana(int32 NewMana);
};