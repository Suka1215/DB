// STEvaluator_PlayerStats.h
#pragma once

#include "CoreMinimal.h"
#include "STEvaluator_EnemyBase.h"
#include "STEvaluator_PlayerStats.generated.h"

/**
 * Evaluator: Tracks player/target stats
 * Allows enemy to make intelligent decisions based on player's condition
 */
UCLASS()
class DAWNBLADE_API USTEvaluator_PlayerStats : public USTEvaluator_EnemyBase
{
    GENERATED_BODY()

protected:
    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
    // ========================================
    // HEALTH & RESOURCES
    // ========================================

    /** Player's current health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float PlayerCurrentHealth = 0.0f;

    /** Player's max health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float PlayerMaxHealth = 0.0f;

    /** Player's health percentage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Health")
    float PlayerHealthPercent = 100.0f;

    /** Player's current mana */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float PlayerCurrentMana = 0.0f;

    /** Player's max mana */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float PlayerMaxMana = 0.0f;

    /** Player's mana percentage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Resources")
    float PlayerManaPercent = 100.0f;

    // ========================================
    // COMBAT STATS
    // ========================================

    /** Player's attack power */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float PlayerAttack = 0.0f;

    /** Player's defense */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Combat")
    float PlayerDefense = 0.0f;

    /** Player's level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Info")
    int32 PlayerLevel = 1;

    // ========================================
    // THREAT ASSESSMENT
    // ========================================

    /** Is player low on health (weak target) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Threat")
    bool bPlayerIsLowHealth = false;

    /** Is player dangerous (high attack relative to enemy defense) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Threat")
    bool bPlayerIsDangerous = false;

    /** Player threat level (0-100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output|Threat")
    float PlayerThreatLevel = 50.0f;

    // ========================================
    // PARAMETERS
    // ========================================

    /** Player low health threshold (percent) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    float PlayerLowHealthThreshold = 30.0f;

    /** Threat multiplier for attack comparison */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
    float ThreatMultiplier = 1.5f;
};