// DBGE_EquipmentStats.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "DBGE_EquipmentStats.generated.h"

/**
 * Gameplay Effect applied when equipment is equipped
 * Modifies character attributes based on equipment stats
 */
UCLASS()
class DAWNBLADE_API UDBGE_EquipmentStats : public UGameplayEffect
{
	GENERATED_BODY()
    
public:
	UDBGE_EquipmentStats()
	{
		// This is an infinite duration effect (lasts until removed)
		DurationPolicy = EGameplayEffectDurationType::Infinite;
	}
};