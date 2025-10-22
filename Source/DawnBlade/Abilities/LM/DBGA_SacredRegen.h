#pragma once

#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_SacredRegen.generated.h"

class UGameplayEffect;

/**
 * LightMage – Sacred Regen
 * Applies a periodic HoT GameplayEffect to allies within an AoE.
 */
UCLASS()
class DAWNBLADE_API UDBGA_SacredRegen : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_SacredRegen();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Query allies around a location (returns Actors directly; no FOverlapResult). */
	TArray<AActor*> GetAlliesInRadius(const FVector& Location) const;

protected:
	/** Periodic HoT GE to apply (consumes SetByCaller.Duration/Healing/Period). */
	UPROPERTY(EditDefaultsOnly, Category="DB|Effects")
	TSubclassOf<UGameplayEffect> GE_SacredRegen;

	/** HoT config */
	UPROPERTY(EditDefaultsOnly, Category="DB|SacredRegen") float HealingPerTick       = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="DB|SacredRegen") float RegenerationDuration = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="DB|SacredRegen") float TickInterval         = 1.f;
	UPROPERTY(EditDefaultsOnly, Category="DB|SacredRegen") float AoERadius            = 600.f;
};
