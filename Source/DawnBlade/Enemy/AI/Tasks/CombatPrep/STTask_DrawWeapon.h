#pragma once
#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_DrawWeapon.generated.h"

/**
 * Play draw weapon montage and set weapon drawn tag
 * Waits for montage to complete
 */
UCLASS()
class DAWNBLADE_API USTTask_DrawWeapon : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_DrawWeapon(const FObjectInitializer& ObjectInitializer);

    /** ✅ Context binding - StateTree will auto-populate this */
    UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
    TObjectPtr<class ADBEnemyCharacter> Actor;

    /** Draw weapon montage (optional, will use ability system if not set) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    TObjectPtr<class UAnimMontage> DrawMontage;

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
    bool bMontageFinished = false;

    UFUNCTION()
    void HandleMontageStarted(UAnimMontage* TheMontage);
    
    UFUNCTION()
    void HandleMontageEnded(UAnimMontage* TheMontage, bool bInterrupted);
};