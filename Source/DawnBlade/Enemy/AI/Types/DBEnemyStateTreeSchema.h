// DBEnemyStateTreeSchema.h
#pragma once

#include "CoreMinimal.h"
#include "DBEnemyStateTreeSchema.generated.h"

/**
 * Schema that defines what data is available to StateTree tasks
 * This is passed to all tasks/conditions automatically
 */
USTRUCT()
struct DAWNBLADE_API FDBEnemyStateTreeSchema
{
	GENERATED_BODY()

	/** The enemy character */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<class ADBEnemyCharacter> EnemyCharacter;

	/** The AI Controller */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<class ADBAIController_Enemy> AIController;

	/** Current target actor */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor;

	/** Last known target location */
	UPROPERTY(EditAnywhere, Category = "Context")
	FVector LastKnownTargetLocation = FVector::ZeroVector;

	/** Random patrol location */
	UPROPERTY(EditAnywhere, Category = "Context")
	FVector PatrolLocation = FVector::ZeroVector;

	/** Home location (spawn point) */
	UPROPERTY(EditAnywhere, Category = "Context")
	FVector HomeLocation = FVector::ZeroVector;
};