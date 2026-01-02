// STEvaluator_EnemyStats.h
#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_EnemyStats.generated.h"

/**
 * Evaluator: Tracks enemy's own combat stats
 * Always up-to-date for decision making
 */
UCLASS()
class DAWNBLADE_API USTEvaluator_EnemyStats : public USTEvaluator_EnemyBase
{
    GENERATED_BODY()

protected:
    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
    // ========================================
    // HEALTH & RESOURCES
    // ========================================

    /** Current health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float CurrentHealth = 0.0f;

    /** Max health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float MaxHealth = 0.0f;

    /** Health percentage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float HealthPercent = 100.0f;

    /** Current mana */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float CurrentMana = 0.0f;

    /** Max mana */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float MaxMana = 0.0f;

    /** Mana percentage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float ManaPercent = 100.0f;

    // ========================================
    // PRIMARY STATS
    // ========================================

    /** Strength */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Strength = 0.0f;

    /** Vitality */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Vitality = 0.0f;

    /** Dexterity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Dexterity = 0.0f;

    /** Agility */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Agility = 0.0f;

    /** Intelligence */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Intelligence = 0.0f;

    /** Mind */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Primary")
    float Mind = 0.0f;

    // ========================================
    // COMBAT STATS
    // ========================================

    /** Attack power */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float Attack = 0.0f;

    /** Defense */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float Defense = 0.0f;

    /** Magic attack */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float MagicAttack = 0.0f;

    /** Magic defense */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float MagicDefense = 0.0f;

    /** Accuracy */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float Accuracy = 0.0f;

    /** Evasion */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float Evasion = 0.0f;

    /** Current TP */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float TP = 0.0f;

    // ========================================
    // STATUS FLAGS
    // ========================================

    /** Is low on health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Status")
    bool bIsLowHealth = false;

    /** Is low on mana */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Status")
    bool bIsLowMana = false;

    /** Has full TP */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Status")
    bool bHasFullTP = false;

    // ========================================
    // PARAMETERS
    // ========================================

    /** Low health threshold (percent) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    float LowHealthThreshold = 30.0f;

    /** Low mana threshold (percent) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    float LowManaThreshold = 20.0f;

    /** Full TP amount */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    float FullTPAmount = 3000.0f;
};