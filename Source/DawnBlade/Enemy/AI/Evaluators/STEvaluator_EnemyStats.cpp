// STEvaluator_EnemyStats.cpp
#include "STEvaluator_EnemyStats.h"
#include "Enemy/DBEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"

void USTEvaluator_EnemyStats::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    Super::Tick(Context, DeltaTime);

    ADBEnemyCharacter* Enemy = GetEnemyCharacter(Context);
    
    if (!Enemy)
    {
        // Reset all values
        CurrentHealth = 0.0f;
        MaxHealth = 0.0f;
        HealthPercent = 0.0f;
        CurrentMana = 0.0f;
        MaxMana = 0.0f;
        ManaPercent = 0.0f;
        return;
    }

    UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    // ========================================
    // PRIMARY ATTRIBUTES (Health, Mana, Stats)
    // ========================================
    const UDBAttr_Primary* PrimaryAttribs = ASC->GetSet<UDBAttr_Primary>();
    if (PrimaryAttribs)
    {
        // Health
        CurrentHealth = PrimaryAttribs->GetHealth();
        MaxHealth = PrimaryAttribs->GetMaxHealth();
        HealthPercent = MaxHealth > 0 ? (CurrentHealth / MaxHealth) * 100.0f : 0.0f;

        // Mana
        CurrentMana = PrimaryAttribs->GetMana();
        MaxMana = PrimaryAttribs->GetMaxMana();
        ManaPercent = MaxMana > 0 ? (CurrentMana / MaxMana) * 100.0f : 0.0f;

        // Primary Stats
        Strength = PrimaryAttribs->GetStrength();
        Vitality = PrimaryAttribs->GetVitality();
        Dexterity = PrimaryAttribs->GetDexterity();
        Agility = PrimaryAttribs->GetAgility();
        Intelligence = PrimaryAttribs->GetIntelligence();
        Mind = PrimaryAttribs->GetMind();
    }

    // ========================================
    // COMBAT ATTRIBUTES
    // ========================================
    const UDBAttr_Combat* CombatAttribs = ASC->GetSet<UDBAttr_Combat>();
    if (CombatAttribs)
    {
        Attack = CombatAttribs->GetAttack();
        Defense = CombatAttribs->GetDefense();
        MagicAttack = CombatAttribs->GetMagicAttack();
        MagicDefense = CombatAttribs->GetMagicDefense();
        Accuracy = CombatAttribs->GetAccuracy();
        Evasion = CombatAttribs->GetEvasion();
        TP = CombatAttribs->GetTP();
    }

    // ========================================
    // STATUS FLAGS
    // ========================================
    bIsLowHealth = HealthPercent <= LowHealthThreshold;
    bIsLowMana = ManaPercent <= LowManaThreshold;
    bHasFullTP = TP >= FullTPAmount;

    // Log (VeryVerbose to avoid spam)
    UE_LOG(LogTemp, VeryVerbose, TEXT("Enemy Stats: HP %.0f/%.0f (%.1f%%) | MP %.0f/%.0f (%.1f%%) | ATK %.0f | DEF %.0f | TP %.0f"),
        CurrentHealth, MaxHealth, HealthPercent,
        CurrentMana, MaxMana, ManaPercent,
        Attack, Defense, TP
    );
}