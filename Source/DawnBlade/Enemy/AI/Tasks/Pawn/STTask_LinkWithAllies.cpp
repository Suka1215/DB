// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_LinkWithAllies.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus USTTask_LinkWithAllies::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!Enemy || !Target) return EStateTreeRunStatus::Failed;

	// Find nearby enemies
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(Enemy->GetWorld(), ADBEnemyCharacter::StaticClass(), FoundActors);

	int32 LinkedCount = 0;

	for (AActor* Actor : FoundActors)
	{
		if (Actor == Enemy) continue; // Skip self
		if (LinkedCount >= MaxAllies) break;

		ADBEnemyCharacter* AllyEnemy = Cast<ADBEnemyCharacter>(Actor);
		if (!AllyEnemy) continue;

		// Check distance
		float Distance = FVector::Dist(Enemy->GetActorLocation(), Actor->GetActorLocation());
		if (Distance > LinkRadius) continue;

		// Check if ally is already engaged
		if (ADBAIController_Enemy* AllyController = Cast<ADBAIController_Enemy>(AllyEnemy->GetController()))
		{
			if (AllyController->GetTargetActor()) continue; // Already has target

			// Alert ally to engage
			AllyController->SetTargetActor(Target);
			LinkedCount++;

			UE_LOG(LogTemp, Warning, TEXT("🔗 %s linked with %s!"), 
				*Enemy->GetName(), 
				*AllyEnemy->GetName());
		}
	}

	if (LinkedCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("🔗 Linked %d allies!"), LinkedCount);
	}

	return EStateTreeRunStatus::Succeeded;
}
