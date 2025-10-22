// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/DBSkillchainSubSystem.h"

void UDBSkillchainSubSystem::RegisterTech(AActor* Target, AActor* Instigator, FGameplayTag TechTag)
{
	if (!Target) return;
	const float Now = Target->GetWorld()->GetTimeSeconds();
	FDBChainState& S = ChainByTarget.FindOrAdd(Target);
	S.LastTech = TechTag; S.Time = Now; S.Instigator = Instigator;
	// TODO: check chain windows + apply bonus GE
}
