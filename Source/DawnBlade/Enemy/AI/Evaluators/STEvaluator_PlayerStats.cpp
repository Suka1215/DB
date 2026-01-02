// STEvaluator_PlayerStats.cpp
#include "STEvaluator_PlayerStats.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "GameFramework/Character.h"

void USTEvaluator_PlayerStats::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    Super::Tick(Context, DeltaTime);

    ADBAIController_Enemy* AIController = GetAIController(Context);
    ADBEnemyCharacter* Enemy = GetEnemyCharacter(Context);
    
    if (!AIController || !Enemy)
    {
        PlayerCurrentHealth = 0.0f;
        PlayerMaxHealth = 0.0f;
        PlayerHealthPercent = 0.0f;
        return;
    }

    // Get target (player)
    AActor* Target = AIController->GetTargetActor();
    if (!Target)
    {
        PlayerCurrentHealth = 0.0f;
        PlayerMaxHealth = 0.0f;
        PlayerHealthPercent = 0.0f;
        return;
    }

    // Try to get player's ability system
    UAbilitySystemComponent* TargetASC = nullptr;
    
    // Check if target has ASC interface
    if (Target->Implements<UAbilitySystemInterface>())
    {
        IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);
        if (ASCInterface)
        {
            TargetASC = ASCInterface->GetAbilitySystemComponent();
        }
    }

    // If no ASC found, try getting from character
    if (!TargetASC)
    {
        ACharacter* TargetCharacter = Cast<ACharacter>(Target);
        if (TargetCharacter && TargetCharacter->Implements<UAbilitySystemInterface>())
        {
            IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetCharacter);
            if (ASCInterface)
            {
                TargetASC = ASCInterface->GetAbilitySystemComponent();
            }
        }
    }

    if (!TargetASC)
    {
        // No GAS on target, can't get stats
        UE_LOG(LogTemp, VeryVerbose, TEXT("Target has no AbilitySystemComponent"));
        return;
    }

    // ========================================
    // GET PLAYER PRIMARY ATTRIBUTES
    // ========================================
    const UDBAttr_Primary* PlayerPrimaryAttribs = TargetASC->GetSet<UDBAttr_Primary>();
    if (PlayerPrimaryAttribs)
    {
        PlayerCurrentHealth = PlayerPrimaryAttribs->GetHealth();
        PlayerMaxHealth = PlayerPrimaryAttribs->GetMaxHealth();
        PlayerHealthPercent = PlayerMaxHealth > 0 ? (PlayerCurrentHealth / PlayerMaxHealth) * 100.0f : 0.0f;

        PlayerCurrentMana = PlayerPrimaryAttribs->GetMana();
        PlayerMaxMana = PlayerPrimaryAttribs->GetMaxMana();
        PlayerManaPercent = PlayerMaxMana > 0 ? (PlayerCurrentMana / PlayerMaxMana) * 100.0f : 0.0f;

        // Get level if available
        PlayerLevel = FMath::RoundToInt(PlayerPrimaryAttribs->GetLevel());
    }

    // ========================================
    // GET PLAYER COMBAT ATTRIBUTES
    // ========================================
    const UDBAttr_Combat* PlayerCombatAttribs = TargetASC->GetSet<UDBAttr_Combat>();
    if (PlayerCombatAttribs)
    {
        PlayerAttack = PlayerCombatAttribs->GetAttack();
        PlayerDefense = PlayerCombatAttribs->GetDefense();
    }

    // ========================================
    // CALCULATE THREAT ASSESSMENT
    // ========================================
    
    // Player is low health if below threshold
    bPlayerIsLowHealth = PlayerHealthPercent <= PlayerLowHealthThreshold;

    // Player is dangerous if their attack is much higher than enemy's defense
    const UDBAttr_Combat* EnemyCombatAttribs = Enemy->GetAbilitySystemComponent()->GetSet<UDBAttr_Combat>();
    if (EnemyCombatAttribs)
    {
        float EnemyDefense = EnemyCombatAttribs->GetDefense();
        bPlayerIsDangerous = PlayerAttack >= (EnemyDefense * ThreatMultiplier);
    }

    // Calculate overall threat level (0-100)
    // Higher if player is high level, high attack, high health
    float LevelThreat = FMath::Clamp(PlayerLevel * 5.0f, 0.0f, 40.0f);
    float AttackThreat = FMath::Clamp(PlayerAttack / 10.0f, 0.0f, 30.0f);
    float HealthThreat = FMath::Clamp(PlayerHealthPercent / 3.0f, 0.0f, 30.0f);
    
    PlayerThreatLevel = LevelThreat + AttackThreat + HealthThreat;

    // Log
    UE_LOG(LogTemp, VeryVerbose, TEXT("Player Stats: HP %.0f/%.0f (%.1f%%) | ATK %.0f | DEF %.0f | Lvl %d | Threat %.0f | Dangerous: %s"),
        PlayerCurrentHealth, PlayerMaxHealth, PlayerHealthPercent,
        PlayerAttack, PlayerDefense, PlayerLevel, PlayerThreatLevel,
        bPlayerIsDangerous ? TEXT("YES") : TEXT("NO")
    );
}