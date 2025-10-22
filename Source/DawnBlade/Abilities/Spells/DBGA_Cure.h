#pragma once
#include "CoreMinimal.h"
#include "Abilities/Base/DBGA_Base.h"
#include "DBGA_Cure.generated.h"

/**
 * Cure I–IV (shared Divine Magic spell).
 * Potency, cost, cast time, and cooldown are pulled from DT_Abilities.
 */
UCLASS()
class DAWNBLADE_API UDBGA_Cure : public UDBGA_Base
{
	GENERATED_BODY()

public:
	UDBGA_Cure();

protected:
	// Cure GameplayEffect (applies the healing)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cure")
	TSubclassOf<class UGameplayEffect> GE_Cure;

	// Ability activation
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Job affinity scaling (LightMage = 1.0, Paladin = 0.7, Warrior = 0.4, etc.) */
	float GetJobHealingMultiplier(const AActor* Caster) const;
};
