// Fill out your copyright notice in the Description page of Project Settings.

#include "DBAttr_Secondary.h"
#include "Net/UnrealNetwork.h"

UDBAttr_Secondary::UDBAttr_Secondary()
{
}

void UDBAttr_Secondary::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Haste, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Slow, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, MovementSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Regen, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Refresh, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Regain, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, HPRecoveredWhileHealing, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, MPRecoveredWhileHealing, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, DamageTaken, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, SpellInterruption, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Secondary, Addle, COND_None, REPNOTIFY_Always);
}

void UDBAttr_Secondary::OnRep_Haste(const FGameplayAttributeData& OldHaste)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Haste, OldHaste);
}

void UDBAttr_Secondary::OnRep_Slow(const FGameplayAttributeData& OldSlow)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Slow, OldSlow);
}

void UDBAttr_Secondary::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, MovementSpeed, OldMovementSpeed);
}

void UDBAttr_Secondary::OnRep_Regen(const FGameplayAttributeData& OldRegen)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Regen, OldRegen);
}

void UDBAttr_Secondary::OnRep_Refresh(const FGameplayAttributeData& OldRefresh)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Refresh, OldRefresh);
}

void UDBAttr_Secondary::OnRep_Regain(const FGameplayAttributeData& OldRegain)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Regain, OldRegain);
}

void UDBAttr_Secondary::OnRep_HPRecoveredWhileHealing(const FGameplayAttributeData& OldHPRecoveredWhileHealing)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, HPRecoveredWhileHealing, OldHPRecoveredWhileHealing);
}

void UDBAttr_Secondary::OnRep_MPRecoveredWhileHealing(const FGameplayAttributeData& OldMPRecoveredWhileHealing)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, MPRecoveredWhileHealing, OldMPRecoveredWhileHealing);
}

void UDBAttr_Secondary::OnRep_DamageTaken(const FGameplayAttributeData& OldDamageTaken)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, DamageTaken, OldDamageTaken);
}

void UDBAttr_Secondary::OnRep_SpellInterruption(const FGameplayAttributeData& OldSpellInterruption)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, SpellInterruption, OldSpellInterruption);
}

void UDBAttr_Secondary::OnRep_Addle(const FGameplayAttributeData& OldAddle)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Secondary, Addle, OldAddle);
}