// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "DBSkillchainSubSystem.generated.h"

USTRUCT()
struct FDBChainState
{
	GENERATED_BODY()
	UPROPERTY() FGameplayTag LastTech;
	UPROPERTY() float Time = 0.f;
	UPROPERTY() TWeakObjectPtr<AActor> Instigator;
};

/**
 * 
 */
UCLASS()
class DAWNBLADE_API UDBSkillchainSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void RegisterTech(AActor* Target, AActor* Instigator, FGameplayTag TechTag);
private:
	TMap<TWeakObjectPtr<AActor>, FDBChainState> ChainByTarget;
	
};
