#pragma once

#include "Engine/DataTable.h"  
#include "EDBActionType.h"
#include "GameplayTagContainer.h"
#include "FDBAbilityData.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FDBAbilityData : public FTableRowBase
{
	GENERATED_BODY()

	// Identity
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName			AbilityName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EDBActionType	ActionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag	SchoolTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32			Rank = 1;

	// Costs
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MPCost	= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TPCost	= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HPCost	= 0.f;

	// Timings & Targeting
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Cooldown		= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CastTime		= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Duration		= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Period		= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Range			= 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AoeRadius		= 0.f;

	// Core effect knobs
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Potency     = 0.f;   // heal/damage/buff amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Multiplier  = 1.f;   // generic extra knob

	// Scaling (which stat drives it)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag ScalingAttribute; // Attributes.MagicAttack, Attributes.Attack, etc.

	// Optional: UI/FX hooks
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag CooldownTag;  // Cooldown.Shared.Cure
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGameplayTag CategoryTag;  // Category.Divine.Healing, etc.
};
