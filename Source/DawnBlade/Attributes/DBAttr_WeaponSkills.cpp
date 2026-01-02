// Fill out your copyright notice in the Description page of Project Settings.

#include "DBAttr_WeaponSkills.h"
#include "Net/UnrealNetwork.h"

UDBAttr_WeaponSkills::UDBAttr_WeaponSkills()
{
}

void UDBAttr_WeaponSkills::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, SwordSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, DaggerSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, AxeSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ClubSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, StaffSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, PolearmSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, KatanaSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, GreatSwordSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, GreatAxeSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, GreatKatanaSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ScytheSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, HandToHandSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ArcherySkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, MarksmanshipSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ThrowingSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ShieldSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, ParryingSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, GuardSkill, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_WeaponSkills, EvasionSkill, COND_None, REPNOTIFY_Always);
}

void UDBAttr_WeaponSkills::OnRep_SwordSkill(const FGameplayAttributeData& OldSwordSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, SwordSkill, OldSwordSkill);
}

void UDBAttr_WeaponSkills::OnRep_DaggerSkill(const FGameplayAttributeData& OldDaggerSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, DaggerSkill, OldDaggerSkill);
}

void UDBAttr_WeaponSkills::OnRep_AxeSkill(const FGameplayAttributeData& OldAxeSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, AxeSkill, OldAxeSkill);
}

void UDBAttr_WeaponSkills::OnRep_ClubSkill(const FGameplayAttributeData& OldClubSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ClubSkill, OldClubSkill);
}

void UDBAttr_WeaponSkills::OnRep_StaffSkill(const FGameplayAttributeData& OldStaffSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, StaffSkill, OldStaffSkill);
}

void UDBAttr_WeaponSkills::OnRep_PolearmSkill(const FGameplayAttributeData& OldPolearmSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, PolearmSkill, OldPolearmSkill);
}

void UDBAttr_WeaponSkills::OnRep_KatanaSkill(const FGameplayAttributeData& OldKatanaSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, KatanaSkill, OldKatanaSkill);
}

void UDBAttr_WeaponSkills::OnRep_GreatSwordSkill(const FGameplayAttributeData& OldGreatSwordSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, GreatSwordSkill, OldGreatSwordSkill);
}

void UDBAttr_WeaponSkills::OnRep_GreatAxeSkill(const FGameplayAttributeData& OldGreatAxeSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, GreatAxeSkill, OldGreatAxeSkill);
}

void UDBAttr_WeaponSkills::OnRep_GreatKatanaSkill(const FGameplayAttributeData& OldGreatKatanaSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, GreatKatanaSkill, OldGreatKatanaSkill);
}

void UDBAttr_WeaponSkills::OnRep_ScytheSkill(const FGameplayAttributeData& OldScytheSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ScytheSkill, OldScytheSkill);
}

void UDBAttr_WeaponSkills::OnRep_HandToHandSkill(const FGameplayAttributeData& OldHandToHandSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, HandToHandSkill, OldHandToHandSkill);
}

void UDBAttr_WeaponSkills::OnRep_ArcherySkill(const FGameplayAttributeData& OldArcherySkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ArcherySkill, OldArcherySkill);
}

void UDBAttr_WeaponSkills::OnRep_MarksmanshipSkill(const FGameplayAttributeData& OldMarksmanshipSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, MarksmanshipSkill, OldMarksmanshipSkill);
}

void UDBAttr_WeaponSkills::OnRep_ThrowingSkill(const FGameplayAttributeData& OldThrowingSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ThrowingSkill, OldThrowingSkill);
}

void UDBAttr_WeaponSkills::OnRep_ShieldSkill(const FGameplayAttributeData& OldShieldSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ShieldSkill, OldShieldSkill);
}

void UDBAttr_WeaponSkills::OnRep_ParryingSkill(const FGameplayAttributeData& OldParryingSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, ParryingSkill, OldParryingSkill);
}

void UDBAttr_WeaponSkills::OnRep_GuardSkill(const FGameplayAttributeData& OldGuardSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, GuardSkill, OldGuardSkill);
}

void UDBAttr_WeaponSkills::OnRep_EvasionSkill(const FGameplayAttributeData& OldEvasionSkill)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_WeaponSkills, EvasionSkill, OldEvasionSkill);
}