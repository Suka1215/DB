#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DBGA_AutoEngage.generated.h"

/**
 * Auto-Engage Ability - Runs continuously, detects enemies and triggers engagement
 * This is the "always-on" detection system that calls DBGA_Engage when appropriate
 */
UCLASS()
class DAWNBLADE_API UDBGA_AutoEngage : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDBGA_AutoEngage();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;

	// How far to detect enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DetectionRange = 2000.0f; // 20 meters

	// How often to check for enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CheckInterval = 0.5f;

	// Engage range (triggers weapon draw and engage ability)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float EngageRange = 600.0f; // 6 meters

private:
	FTimerHandle CheckTimer;
	void CheckForEnemies();
    
	AActor* LastDetectedTarget;
	bool bHasEngaged;
};