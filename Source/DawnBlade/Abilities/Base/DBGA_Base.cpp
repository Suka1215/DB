// DBGA_Base.cpp

#include "Abilities/Base/DBGA_Base.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

// SetByCaller tag helpers
static FGameplayTag TAG_SetByCaller_Duration() { return FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Duration")); }
static FGameplayTag TAG_SetByCaller_Cooldown() { return FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Cooldown")); }
static FGameplayTag TAG_SetByCaller_MP()       { return FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.MP")); }
static FGameplayTag TAG_SetByCaller_TP()       { return FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.TP")); }

UDBGA_Base::UDBGA_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

const FDBAbilityData* UDBGA_Base::GetAbilityData() const
{
	if (!AbilityDataTable || AbilityRowName.IsNone())
	{
		return nullptr;
	}

	return AbilityDataTable->FindRow<FDBAbilityData>(AbilityRowName, TEXT("DBGA_Base Lookup"));
}


bool UDBGA_Base::PayCostsAndLock(const FGameplayAbilitySpecHandle& Handle,
								 const FGameplayAbilityActorInfo* Info,
								 const FGameplayAbilityActivationInfo& ActivationInfo)
{
	if (!CommitCheck(Handle, Info, ActivationInfo))
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	// MP cost
	if (!FMath::IsNearlyZero(MPCost))
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(UGameplayEffect::StaticClass(), 1.f, ASC->MakeEffectContext());
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(TAG_SetByCaller_MP(), -MPCost);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	// TP cost
	if (!FMath::IsNearlyZero(TPCost))
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(UGameplayEffect::StaticClass(), 1.f, ASC->MakeEffectContext());
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(TAG_SetByCaller_TP(), -TPCost);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	// Short action lock
	if (ActionLockSeconds > 0.f)
	{
		if (GE_ActiveWindow)
		{
			FGameplayEffectSpecHandle Lock = ASC->MakeOutgoingSpec(GE_ActiveWindow, 1.f, ASC->MakeEffectContext());
			if (Lock.IsValid())
			{
				Lock.Data->SetSetByCallerMagnitude(TAG_SetByCaller_Duration(), ActionLockSeconds);
				ASC->ApplyGameplayEffectSpecToSelf(*Lock.Data.Get());
			}
		}
		else
		{
			// Fallback to a generic GE if no specific ActiveWindow GE set
			FGameplayEffectSpecHandle Lock = ASC->MakeOutgoingSpec(UGameplayEffect::StaticClass(), 1.f, ASC->MakeEffectContext());
			if (Lock.IsValid())
			{
				Lock.Data->SetSetByCallerMagnitude(TAG_SetByCaller_Duration(), ActionLockSeconds);
				ASC->ApplyGameplayEffectSpecToSelf(*Lock.Data.Get());
			}
		}
	}

	// Tell GAS we’re committing (spends costs, starts cooldown tracking)
	if (!CommitAbility(Handle, Info, ActivationInfo))
	{
		return false;
	}

	// Optional "active window" beyond the initial lock
	if (GE_ActiveWindow && DurationSeconds > 0.f)
	{
		FGameplayEffectSpecHandle Active = ASC->MakeOutgoingSpec(GE_ActiveWindow, 1.f, ASC->MakeEffectContext());
		if (Active.IsValid())
		{
			Active.Data->SetSetByCallerMagnitude(TAG_SetByCaller_Duration(), DurationSeconds);
			ASC->ApplyGameplayEffectSpecToSelf(*Active.Data.Get());
		}
	}

	// Cooldown effect
	if (GE_Cooldown && CooldownSeconds > 0.f)
	{
		FGameplayEffectSpecHandle CD = ASC->MakeOutgoingSpec(GE_Cooldown, 1.f, ASC->MakeEffectContext());
		if (CD.IsValid())
		{
			CD.Data->SetSetByCallerMagnitude(TAG_SetByCaller_Cooldown(), CooldownSeconds);
			ASC->ApplyGameplayEffectSpecToSelf(*CD.Data.Get());
		}
	}

	return true;
}

/** UE override — pointer-returning version used by GAS/UI. */
const FGameplayTagContainer* UDBGA_Base::GetCooldownTags() const
{
	CooldownTagsCache.Reset();
	if (CooldownTag.IsValid())
	{
		CooldownTagsCache.AddTag(CooldownTag);
	}
	return &CooldownTagsCache;
}

/** Your original helper — fill an output container if you prefer that callsite. */
void UDBGA_Base::GetCooldownTags(FGameplayTagContainer& OutTags) const
{
	OutTags.Reset();
	if (CooldownTag.IsValid())
	{
		OutTags.AddTag(CooldownTag);
	}
}
