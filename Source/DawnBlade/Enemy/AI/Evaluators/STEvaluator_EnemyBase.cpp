// STEvaluator_EnemyBase.cpp
#include "STEvaluator_EnemyBase.h"

#include "StateTreeExecutionContext.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/DBAIController_Enemy.h"

ADBAIController_Enemy* USTEvaluator_EnemyBase::GetAIController(FStateTreeExecutionContext& Context) const
{
	return Cast<ADBAIController_Enemy>(Context.GetOwner());
}

ADBEnemyCharacter* USTEvaluator_EnemyBase::GetEnemyCharacter(FStateTreeExecutionContext& Context) const
{
	ADBAIController_Enemy* AIController = GetAIController(Context);
	return AIController ? AIController->GetEnemyCharacter() : nullptr;
}