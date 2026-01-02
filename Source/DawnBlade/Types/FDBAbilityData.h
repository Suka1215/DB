#pragma once

#include "Engine/DataTable.h"  
#include "EDBActionType.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "FDBAbilityData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FDBAbilityData : public FTableRowBase
{
    GENERATED_BODY()

    // ========== IDENTITY ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FName AbilityName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    FText Description;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EDBActionType ActionType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity", meta=(Categories="School"))
    FGameplayTagContainer SchoolTags;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EDBSpellType SpellType = EDBSpellType::None;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    EDBSpellEffect SpellEffect = EDBSpellEffect::None;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
    int32 Rank = 1;

    // ========== COSTS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Costs")
    float MPCost = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Costs")
    float TPCost = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Costs")
    float HPCost = 0.f;

    // ========== TIMING ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    float Cooldown = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    float CastTime = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    float Duration = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    float Period = 0.f;

    // ========== RANGE & TARGETING ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Range")
    float Range = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Range")
    float AoeRadius = 0.f;

    // ========== POWER ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power")
    float Potency = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power")
    float Multiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power", meta=(Categories="Attributes"))
    FGameplayTag ScalingAttribute;

    // ========== ANIMATION & VISUALS ========== ← ADD THIS!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    TSoftObjectPtr<UAnimMontage> AbilityMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visuals")
    TSoftObjectPtr<UParticleSystem> CastVFX;  // VFX during cast
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visuals")
    TSoftObjectPtr<UParticleSystem> HitVFX;   // VFX on impact
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
    TSoftObjectPtr<USoundBase> CastSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
    TSoftObjectPtr<USoundBase> HitSound;
    
    // ========== UI ========== ← ADD THIS!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSoftObjectPtr<UTexture2D> Icon;
    
    // ========== GAMEPLAY EFFECT ========== ← ADD THIS!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameplayEffect")
    TSubclassOf<UGameplayEffect> GameplayEffectClass;

    // ========== TAGS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags", meta=(Categories="Cooldown"))
    FGameplayTag CooldownTag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags", meta=(Categories="Category"))
    FGameplayTag CategoryTag;
};