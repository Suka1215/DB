#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DBGE_Cure.generated.h"

/**
 * Cure GameplayEffect
 * - Instant, additive heal
 * - Magnitude is provided by the GA via SetByCaller tag: "SetByCaller.Healing"
 */
UCLASS(BlueprintType)
class DAWNBLADE_API UDBGE_Cure : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UDBGE_Cure();
};
