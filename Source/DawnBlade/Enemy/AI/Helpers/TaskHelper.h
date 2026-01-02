#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "StateTreeExecutionContext.h"

class UDBTagStateComponent;
class UDBCombatComponent;
class UAbilitySystemComponent;

namespace STTaskHelpers
{
	/**
	 * Get Tag State Component (works on ANY actor)
	 */
	template<typename TInstanceData>
	static UDBTagStateComponent* GetTagState(FStateTreeExecutionContext& Context, const TInstanceData& InstanceData)
	{
		AActor* Actor = InstanceData.Enemy ? InstanceData.Enemy : InstanceData.Target;
        
		if (!Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("STTaskHelpers: Actor is null"));
			return nullptr;
		}

		UDBTagStateComponent* TagState = Actor->FindComponentByClass<UDBTagStateComponent>();
        
		if (!TagState)
		{
			UE_LOG(LogTemp, Warning, TEXT("STTaskHelpers: %s missing TagStateComponent!"), *Actor->GetName());
		}

		return TagState;
	}

	/**
	 * Get Combat Component
	 */
	template<typename TInstanceData>
	static UDBCombatComponent* GetCombat(FStateTreeExecutionContext& Context, const TInstanceData& InstanceData)
	{
		AActor* Actor = InstanceData.Enemy ? InstanceData.Enemy : InstanceData.Target;
		return Actor ? Actor->FindComponentByClass<UDBCombatComponent>() : nullptr;
	}

	/**
	 * Get Ability System Component
	 */
	template<typename TInstanceData>
	static UAbilitySystemComponent* GetASC(FStateTreeExecutionContext& Context, const TInstanceData& InstanceData)
	{
		AActor* Actor = InstanceData.Enemy ? InstanceData.Enemy : InstanceData.Target;
        
		if (!Actor) return nullptr;

		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			return ASI->GetAbilitySystemComponent();
		}

		return nullptr;
	}
}