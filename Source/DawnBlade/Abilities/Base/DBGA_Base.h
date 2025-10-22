// DBGA_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Types/FDBAbilityData.h"
#include "DBGA_Base.generated.h"

/**
 * Base class for DawnBlade Gameplay Abilities.
 * Provides shared knobs (costs, durations, range, cooldown) and helpers.
 */
UCLASS()
class DAWNBLADE_API UDBGA_Base : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDBGA_Base();

	UPROPERTY(EditDefaultsOnly, Category="DB|Data")
	UDataTable* AbilityDataTable;

	UPROPERTY(EditDefaultsOnly, Category="DB|Data")
	FName AbilityRowName;

	/** Bring base overload into scope to avoid "function is hidden" warnings. */
	using UGameplayAbility::GetCooldownTags;

	/** UE override — lets GAS/UI query this ability's cooldown tags. */
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	/** Convenience helper that fills an output container (your original API). */
	void GetCooldownTags(FGameplayTagContainer& OutTags) const;

	/** Single cooldown tag this ability uses (set per derived class). */
	UPROPERTY(EditDefaultsOnly, Category="DB|Cooldown")
	FGameplayTag CooldownTag;

protected:
	const FDBAbilityData* GetAbilityData() const;
	
	/** Short action lock / cast time in seconds. */
	UPROPERTY(EditDefaultsOnly, Category="DB|Costs")
	float ActionLockSeconds = 0.4f;

	/** Mana cost. */
	UPROPERTY(EditDefaultsOnly, Category="DB|Costs")
	float MPCost = 0.f;

	/** Tactical Points cost. */
	UPROPERTY(EditDefaultsOnly, Category="DB|Costs")
	float TPCost = 0.f;

	/** Targeting/cast range in cm. */
	UPROPERTY(EditDefaultsOnly, Category="DB|Targeting")
	float Range = 0.f;

	/** Duration window (if the ability has an "active" phase). */
	UPROPERTY(EditDefaultsOnly, Category="DB|Duration")
	float DurationSeconds = 0.f;

	/** GE applied to represent the active window (consumes SetByCaller.Duration). */
	UPROPERTY(EditDefaultsOnly, Category="DB|Duration")
	TSubclassOf<class UGameplayEffect> GE_ActiveWindow;

	/** Cooldown duration in seconds. */
	UPROPERTY(EditDefaultsOnly, Category="DB|Cooldown")
	float CooldownSeconds = 0.f;

	/** GE applied as the cooldown (consumes SetByCaller.Cooldown). */
	UPROPERTY(EditDefaultsOnly, Category="DB|Cooldown")
	TSubclassOf<class UGameplayEffect> GE_Cooldown;

	/** Pays MP/TP, applies action lock, active window, and cooldown. */
	bool PayCostsAndLock(const FGameplayAbilitySpecHandle& Handle,
						 const FGameplayAbilityActorInfo* Info,
						 const FGameplayAbilityActivationInfo& ActivationInfo);

	/** Small cache used by the pointer-returning override. */
	UPROPERTY(Transient)
	mutable FGameplayTagContainer CooldownTagsCache;
};
