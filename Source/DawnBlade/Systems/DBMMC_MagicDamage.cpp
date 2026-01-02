#include "DBMMC_MagicDamage.h"
#include "DawnBlade/Attributes/DBAttr_Combat.h"
#include "DawnBlade/Attributes/DBAttr_Primary.h"
#include "DawnBlade/Attributes/DBAttr_MagicSkills.h"

UDBMMC_MagicDamage::UDBMMC_MagicDamage()
{
    // Capture caster's Magic Accuracy
    MagicAccuracyDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Combat::GetMagicAccuracyAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    // Capture target's Magic Evasion
    MagicEvasionDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Combat::GetMagicEvasionAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    );
    
    // Capture caster's INT (for damage calculation - used in elemental magic)
    IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Primary::GetIntelligenceAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    // Capture target's INT (for resistance calculation)
    TargetIntelligenceDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Primary::GetIntelligenceAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    );
    
    // Capture caster's MND (for healing/white magic)
    MindDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Primary::GetMindAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    // Capture target's MND (for white magic resistance)
    TargetMindDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_Primary::GetMindAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    );
    
    // Capture relevant magic skill (will be set via SetByCaller or tag)
    // Examples: ElementalMagicSkill, EnfeeblingMagicSkill, etc.
    ElementalMagicSkillDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_MagicSkills::GetElementalMagicSkillAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    EnfeeblingMagicSkillDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_MagicSkills::GetEnfeeblingMagicSkillAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    HealingMagicSkillDef = FGameplayEffectAttributeCaptureDefinition(
        UDBAttr_MagicSkills::GetHealingMagicSkillAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    );
    
    // Register all capture definitions
    RelevantAttributesToCapture.Add(MagicAccuracyDef);
    RelevantAttributesToCapture.Add(MagicEvasionDef);
    RelevantAttributesToCapture.Add(IntelligenceDef);
    RelevantAttributesToCapture.Add(TargetIntelligenceDef);
    RelevantAttributesToCapture.Add(MindDef);
    RelevantAttributesToCapture.Add(TargetMindDef);
    RelevantAttributesToCapture.Add(ElementalMagicSkillDef);
    RelevantAttributesToCapture.Add(EnfeeblingMagicSkillDef);
    RelevantAttributesToCapture.Add(HealingMagicSkillDef);
}

float UDBMMC_MagicDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;
    
    float MagicAccuracy = 0.f;
    GetCapturedAttributeMagnitude(MagicAccuracyDef, Spec, EvaluationParameters, MagicAccuracy);
    
    float MagicEvasion = 0.f;
    GetCapturedAttributeMagnitude(MagicEvasionDef, Spec, EvaluationParameters, MagicEvasion);
    
    float CasterINT = 0.f;
    GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluationParameters, CasterINT);
    
    float TargetINT = 0.f;
    GetCapturedAttributeMagnitude(TargetIntelligenceDef, Spec, EvaluationParameters, TargetINT);
    
    float CasterMND = 0.f;
    GetCapturedAttributeMagnitude(MindDef, Spec, EvaluationParameters, CasterMND);
    
    float TargetMND = 0.f;
    GetCapturedAttributeMagnitude(TargetMindDef, Spec, EvaluationParameters, TargetMND);
    
    // Determine which magic skill to use based on spell type (via tags or SetByCaller)
    float MagicSkill = 0.f;
    
    // Check tags to determine magic type
    if (SourceTags && SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Magic.Type.Elemental"))))
    {
        GetCapturedAttributeMagnitude(ElementalMagicSkillDef, Spec, EvaluationParameters, MagicSkill);
    }
    else if (SourceTags && SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Magic.Type.Enfeebling"))))
    {
        GetCapturedAttributeMagnitude(EnfeeblingMagicSkillDef, Spec, EvaluationParameters, MagicSkill);
    }
    else if (SourceTags && SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Magic.Type.Healing"))))
    {
        GetCapturedAttributeMagnitude(HealingMagicSkillDef, Spec, EvaluationParameters, MagicSkill);
    }
    
    // Get base spell power from SetByCaller (Thunder I = 50, Thunder II = 100, etc.)
    float BaseSpellPower = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Base")), false, 0.f);
    
    // === FFXI-STYLE MAGIC DAMAGE CALCULATION ===
    
    // 1. Calculate dINT (difference between caster and target INT)
    float dINT = CasterINT - TargetINT;
    dINT = FMath::Clamp(dINT, -100.f, 100.f); // Cap dINT
    
    // 2. Magic damage formula: Base * (1 + (dINT / 100)) * (MagicSkill / 100)
    float Damage = BaseSpellPower * (1.0f + (dINT / 100.0f)) * (MagicSkill / 200.0f);
    
    // 3. Apply magic accuracy vs evasion for resist chance (you can expand this)
    float HitChance = MagicAccuracy - MagicEvasion;
    // For now, just ensure we hit (you can add resist logic later)
    
    // 4. Add some variance (±10%)
    float Variance = FMath::RandRange(0.9f, 1.1f);
    Damage *= Variance;
    
    return FMath::Max(Damage, 0.f);
}