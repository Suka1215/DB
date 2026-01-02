// Fill out your copyright notice in the Description page of Project Settings.

#include "DBAttr_MagicSkills.h"
#include "Net/UnrealNetwork.h"

UDBAttr_MagicSkills::UDBAttr_MagicSkills()
{
}

void UDBAttr_MagicSkills::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, DivineMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, HealingMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, EnhancingMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, EnfeeblingMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, ElementalMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, DarkMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, SummoningMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, NinjutsuSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, SingingSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, StringInstrumentSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, WindInstrumentSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, BlueMagicSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, GeomancySkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_MagicSkills, HandbellSkill, COND_None, REPNOTIFY_Always);
}

void UDBAttr_MagicSkills::OnRep_DivineMagicSkill(const FGameplayAttributeData& OldDivineMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, DivineMagicSkill, OldDivineMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_HealingMagicSkill(const FGameplayAttributeData& OldHealingMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, HealingMagicSkill, OldHealingMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_EnhancingMagicSkill(const FGameplayAttributeData& OldEnhancingMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, EnhancingMagicSkill, OldEnhancingMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_EnfeeblingMagicSkill(const FGameplayAttributeData& OldEnfeeblingMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, EnfeeblingMagicSkill, OldEnfeeblingMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_ElementalMagicSkill(const FGameplayAttributeData& OldElementalMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, ElementalMagicSkill, OldElementalMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_DarkMagicSkill(const FGameplayAttributeData& OldDarkMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, DarkMagicSkill, OldDarkMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_SummoningMagicSkill(const FGameplayAttributeData& OldSummoningMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, SummoningMagicSkill, OldSummoningMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_NinjutsuSkill(const FGameplayAttributeData& OldNinjutsuSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, NinjutsuSkill, OldNinjutsuSkill);
}

void UDBAttr_MagicSkills::OnRep_SingingSkill(const FGameplayAttributeData& OldSingingSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, SingingSkill, OldSingingSkill);
}

void UDBAttr_MagicSkills::OnRep_StringInstrumentSkill(const FGameplayAttributeData& OldStringInstrumentSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, StringInstrumentSkill, OldStringInstrumentSkill);
}

void UDBAttr_MagicSkills::OnRep_WindInstrumentSkill(const FGameplayAttributeData& OldWindInstrumentSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, WindInstrumentSkill, OldWindInstrumentSkill);
}

void UDBAttr_MagicSkills::OnRep_BlueMagicSkill(const FGameplayAttributeData& OldBlueMagicSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, BlueMagicSkill, OldBlueMagicSkill);
}

void UDBAttr_MagicSkills::OnRep_GeomancySkill(const FGameplayAttributeData& OldGeomancySkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, GeomancySkill, OldGeomancySkill);
}

void UDBAttr_MagicSkills::OnRep_HandbellSkill(const FGameplayAttributeData& OldHandbellSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_MagicSkills, HandbellSkill, OldHandbellSkill);
}