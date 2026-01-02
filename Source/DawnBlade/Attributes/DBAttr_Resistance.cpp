// Fill out your copyright notice in the Description page of Project Settings.

#include "DBAttr_Resistance.h"
#include "Net/UnrealNetwork.h"

UDBAttr_Resistance::UDBAttr_Resistance()
{
}

void UDBAttr_Resistance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, FireResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, IceResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, WindResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, EarthResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, LightningResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, WaterResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, LightResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Resistance, DarkResistance, COND_None, REPNOTIFY_Always);
}

void UDBAttr_Resistance::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, FireResistance, OldFireResistance);
}

void UDBAttr_Resistance::OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, IceResistance, OldIceResistance);
}

void UDBAttr_Resistance::OnRep_WindResistance(const FGameplayAttributeData& OldWindResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, WindResistance, OldWindResistance);
}

void UDBAttr_Resistance::OnRep_EarthResistance(const FGameplayAttributeData& OldEarthResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, EarthResistance, OldEarthResistance);
}

void UDBAttr_Resistance::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, LightningResistance, OldLightningResistance);
}

void UDBAttr_Resistance::OnRep_WaterResistance(const FGameplayAttributeData& OldWaterResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, WaterResistance, OldWaterResistance);
}

void UDBAttr_Resistance::OnRep_LightResistance(const FGameplayAttributeData& OldLightResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, LightResistance, OldLightResistance);
}

void UDBAttr_Resistance::OnRep_DarkResistance(const FGameplayAttributeData& OldDarkResistance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Resistance, DarkResistance, OldDarkResistance);
}