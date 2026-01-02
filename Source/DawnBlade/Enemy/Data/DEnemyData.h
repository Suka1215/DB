// DEnemyData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "DEnemyData.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
    Beast       UMETA(DisplayName = "Beast"),
    Undead      UMETA(DisplayName = "Undead"),
    Demon       UMETA(DisplayName = "Demon"),
    Construct   UMETA(DisplayName = "Construct"),
    Dragon      UMETA(DisplayName = "Dragon"),
    Humanoid    UMETA(DisplayName = "Humanoid"),
    Plant       UMETA(DisplayName = "Plant"),
    Elemental   UMETA(DisplayName = "Elemental")
};

UENUM(BlueprintType)
enum class EEnemyBehavior : uint8
{
    Passive     UMETA(DisplayName = "Passive - Won't attack unless provoked"),
    Defensive   UMETA(DisplayName = "Defensive - Attacks if you get too close"),
    Aggressive  UMETA(DisplayName = "Aggressive - Attacks on sight"),
    Linking     UMETA(DisplayName = "Linking - Calls nearby allies when attacked")
};

UENUM(BlueprintType)
enum class EEnemySize : uint8
{
    Small       UMETA(DisplayName = "Small (Rabbit, Imp)"),
    Medium      UMETA(DisplayName = "Medium (Wolf, Goblin)"),
    Large       UMETA(DisplayName = "Large (Ogre, Drake)"),
    Huge        UMETA(DisplayName = "Huge (Dragon, Giant)")
};

UENUM(BlueprintType)
enum class EDetectionType : uint8
{
    Sight       UMETA(DisplayName = "Sight - Cone-based vision"),
    Sound       UMETA(DisplayName = "Sound - 360° hearing"),
    TrueSight   UMETA(DisplayName = "True Sight - 360° vision, ignores stealth")
};

USTRUCT(BlueprintType)
struct FEnemyAttackPattern
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FName AttackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TObjectPtr<USoundBase> AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float BaseDamage = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float AttackRange = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float Cooldown = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float SelectionWeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FGameplayTagContainer AttackTags;
};

/**
 * Enemy Data Asset - Contains STATS and AI configuration only.
 * Visuals (mesh, anim BP, capsule) are set in Blueprint child classes.
 */
UCLASS(BlueprintType)
class DAWNBLADE_API UDEnemyData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // ========================================
    // BASIC INFO
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FText EnemyName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (MultiLine = true))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    EEnemyType EnemyType = EEnemyType::Beast;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    EEnemyBehavior Behavior = EEnemyBehavior::Aggressive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    EEnemySize SizeCategory = EEnemySize::Medium;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FGameplayTagContainer GameplayTags;

    // ========================================
    // STATS
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseHealth = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseMana = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Strength = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Vitality = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Dexterity = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Agility = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Intelligence = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Mind = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Charisma = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Attack = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Evasion = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Accuracy = 95.0f;

    // ========================================
    // RESISTANCES
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float FireResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float IceResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float LightningResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float WaterResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float LightResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resistances", meta = (ClampMin = "-100", ClampMax = "100"))
    float DarkResistance = 0.0f;

    // ========================================
    // AI - DETECTION
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection")
    EDetectionType DetectionMethod = EDetectionType::Sight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection", 
        meta = (EditCondition = "DetectionMethod == EDetectionType::Sight", EditConditionHides, ClampMin = "0", ClampMax = "5000"))
    float SightRange = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection", 
        meta = (EditCondition = "DetectionMethod == EDetectionType::Sight", EditConditionHides, ClampMin = "0", ClampMax = "180"))
    float VisionAngle = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection", 
        meta = (EditCondition = "DetectionMethod == EDetectionType::Sound", EditConditionHides, ClampMin = "0", ClampMax = "3000"))
    float HearingRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection", 
        meta = (EditCondition = "DetectionMethod == EDetectionType::TrueSight", EditConditionHides, ClampMin = "0", ClampMax = "5000"))
    float TrueSightRange = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection", 
        meta = (EditCondition = "DetectionMethod == EDetectionType::TrueSight", EditConditionHides, ClampMin = "0", ClampMax = "3000"))
    float TrueSightHearingRange = 1000.0f;

    // ========================================
    // AI - BEHAVIOR
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "3000"))
    float AggroRange = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "1000"))
    float LinkRange = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float MovementSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    TObjectPtr<class UStateTree> StateTreeAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float DeathRemovalTime = 5.0f;

    // ========================================
    // AI - DEBUG
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Debug")
    bool bShowDebugLines = false;

    // ========================================
    // COMBAT
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TArray<FEnemyAttackPattern> AttackPatterns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackInterval = 2.5f;

    // ========================================
    // ABILITIES
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> SpecialAbilities;

    // ========================================
    // REWARDS
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    int32 ExperienceReward = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    int32 GilReward = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    TObjectPtr<class UDataTable> LootTable;

    // ========================================
    // AUDIO
    // ========================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> IdleSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> AggroSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<USoundBase> DeathSound;

    // ========================================
    // HELPERS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Combat")
    FEnemyAttackPattern GetRandomAttackPattern() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    FEnemyAttackPattern GetAttackPatternByName(FName AttackName) const;
};