// DBAttr_Combat.cpp
#include "DBAttr_Combat.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDBAttr_Combat::UDBAttr_Combat()
{
}

void UDBAttr_Combat::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    
    // Clamp TP to max
    if (Attribute == GetTPAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxTP());
    }
}

void UDBAttr_Combat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    // Handle max TP changes affecting current TP
    if (Data.EvaluatedData.Attribute == GetMaxTPAttribute())
    {
        SetTP(FMath::Clamp(GetTP(), 0.0f, GetMaxTP()));
    }
}

void UDBAttr_Combat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Offensive Stats
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, Attack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, RangedAttack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, Accuracy, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, RangedAccuracy, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, CriticalHitRate, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, CriticalHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, TP, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, MaxTP, COND_None, REPNOTIFY_Always);
    
    // Defensive Stats
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, Defense, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, Evasion, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, MagicEvasion, COND_None, REPNOTIFY_Always);
    
    // Magic Stats
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, MagicAccuracy, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, MagicAttack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, MagicDefense, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, CurePotency, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, PotencyOfCureEffectReceived, COND_None, REPNOTIFY_Always);
    
    // Threat
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Combat, Enmity, COND_None, REPNOTIFY_Always);
}

// ========================================
// OFFENSIVE STATS
// ========================================

void UDBAttr_Combat::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, Attack, OldAttack);
}

void UDBAttr_Combat::OnRep_RangedAttack(const FGameplayAttributeData& OldRangedAttack)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, RangedAttack, OldRangedAttack);
}

void UDBAttr_Combat::OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, Accuracy, OldAccuracy);
}

void UDBAttr_Combat::OnRep_RangedAccuracy(const FGameplayAttributeData& OldRangedAccuracy)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, RangedAccuracy, OldRangedAccuracy);
}

void UDBAttr_Combat::OnRep_CriticalHitRate(const FGameplayAttributeData& OldCriticalHitRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, CriticalHitRate, OldCriticalHitRate);
}

void UDBAttr_Combat::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, CriticalHitDamage, OldCriticalHitDamage);
}

void UDBAttr_Combat::OnRep_TP(const FGameplayAttributeData& OldTP)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, TP, OldTP);
}

void UDBAttr_Combat::OnRep_MaxTP(const FGameplayAttributeData& OldMaxTP)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, MaxTP, OldMaxTP);
}

// ========================================
// DEFENSIVE STATS
// ========================================

void UDBAttr_Combat::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, Defense, OldDefense);
}

void UDBAttr_Combat::OnRep_Evasion(const FGameplayAttributeData& OldEvasion)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, Evasion, OldEvasion);
}

void UDBAttr_Combat::OnRep_MagicEvasion(const FGameplayAttributeData& OldMagicEvasion)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, MagicEvasion, OldMagicEvasion);
}

// ========================================
// MAGIC STATS
// ========================================

void UDBAttr_Combat::OnRep_MagicAccuracy(const FGameplayAttributeData& OldMagicAccuracy)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, MagicAccuracy, OldMagicAccuracy);
}

void UDBAttr_Combat::OnRep_MagicAttack(const FGameplayAttributeData& OldMagicAttack)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, MagicAttack, OldMagicAttack);
}

void UDBAttr_Combat::OnRep_MagicDefense(const FGameplayAttributeData& OldMagicDefense)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, MagicDefense, OldMagicDefense);
}

void UDBAttr_Combat::OnRep_CurePotency(const FGameplayAttributeData& OldCurePotency)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, CurePotency, OldCurePotency);
}

void UDBAttr_Combat::OnRep_PotencyOfCureEffectReceived(const FGameplayAttributeData& OldPotencyOfCureEffectReceived)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, PotencyOfCureEffectReceived, OldPotencyOfCureEffectReceived);
}

// ========================================
// THREAT
// ========================================

void UDBAttr_Combat::OnRep_Enmity(const FGameplayAttributeData& OldEnmity)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Combat, Enmity, OldEnmity);
}