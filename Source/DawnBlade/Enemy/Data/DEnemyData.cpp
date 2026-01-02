// DEnemyData.cpp
#include "DEnemyData.h"

FEnemyAttackPattern UDEnemyData::GetRandomAttackPattern() const
{
	if (AttackPatterns.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ No attack patterns defined for %s"), *EnemyName.ToString());
		return FEnemyAttackPattern();
	}

	// Calculate total weight
	float TotalWeight = 0.0f;
	for (const FEnemyAttackPattern& Pattern : AttackPatterns)
	{
		TotalWeight += Pattern.SelectionWeight;
	}

	if (TotalWeight <= 0.0f)
	{
		return AttackPatterns[0];
	}

	// Random selection based on weight
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;

	for (const FEnemyAttackPattern& Pattern : AttackPatterns)
	{
		CurrentWeight += Pattern.SelectionWeight;
		if (RandomValue <= CurrentWeight)
		{
			return Pattern;
		}
	}

	return AttackPatterns[0];
}

FEnemyAttackPattern UDEnemyData::GetAttackPatternByName(FName AttackName) const
{
	for (const FEnemyAttackPattern& Pattern : AttackPatterns)
	{
		if (Pattern.AttackName == AttackName)
		{
			return Pattern;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("⚠️ Attack pattern '%s' not found for %s"), 
		*AttackName.ToString(), *EnemyName.ToString());
    
	return FEnemyAttackPattern();
}