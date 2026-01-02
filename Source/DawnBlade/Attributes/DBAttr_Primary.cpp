// DBAttr_Primary.cpp
#include "DBAttr_Primary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDBAttr_Primary::UDBAttr_Primary()
{
}

void UDBAttr_Primary::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    
    // Clamp current values to their max
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
}

void UDBAttr_Primary::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    // Handle max changes affecting current values
    if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
}

void UDBAttr_Primary::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, SubjobLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Strength, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Dexterity, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Vitality, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Agility, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Intelligence, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Mind, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Primary, Charisma, COND_None, REPNOTIFY_Always);
}

void UDBAttr_Primary::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Level, OldLevel);
}

void UDBAttr_Primary::OnRep_SubjobLevel(const FGameplayAttributeData& OldSubjobLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, SubjobLevel, OldSubjobLevel);
}

void UDBAttr_Primary::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Health, OldHealth);
}

void UDBAttr_Primary::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, MaxHealth, OldMaxHealth);
}

void UDBAttr_Primary::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Mana, OldMana);
}

void UDBAttr_Primary::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, MaxMana, OldMaxMana);
}

void UDBAttr_Primary::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Strength, OldStrength);
}

void UDBAttr_Primary::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Dexterity, OldDexterity);
}

void UDBAttr_Primary::OnRep_Vitality(const FGameplayAttributeData& OldVitality)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Vitality, OldVitality);
}

void UDBAttr_Primary::OnRep_Agility(const FGameplayAttributeData& OldAgility)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Agility, OldAgility);
}

void UDBAttr_Primary::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Intelligence, OldIntelligence);
}

void UDBAttr_Primary::OnRep_Mind(const FGameplayAttributeData& OldMind)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Mind, OldMind);
}

void UDBAttr_Primary::OnRep_Charisma(const FGameplayAttributeData& OldCharisma)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UDBAttr_Primary, Charisma, OldCharisma);
}