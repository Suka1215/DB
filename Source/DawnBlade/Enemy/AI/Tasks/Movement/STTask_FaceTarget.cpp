// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_FaceTarget.h"

EStateTreeRunStatus USTTask_FaceTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	TimeElapsed = 0.0f;

	if (!Enemy || !Target) return EStateTreeRunStatus::Failed;

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTTask_FaceTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (!Enemy || !Target) return EStateTreeRunStatus::Failed;

	TimeElapsed += DeltaTime;

	// Check timeout
	if (TimeElapsed >= Timeout)
	{
		UE_LOG(LogTemp, Warning, TEXT("⏱️ Face target timeout"));
		return EStateTreeRunStatus::Succeeded;
	}

	// Calculate direction to target
	FVector Direction = (Target->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D();
	FRotator TargetRotation = Direction.Rotation();
	FRotator CurrentRotation = Enemy->GetActorRotation();

	// Check if already facing target
	float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
	if (AngleDiff <= AcceptableAngle)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Facing target"));
		return EStateTreeRunStatus::Succeeded;
	}

	// Rotate towards target
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed / 180.0f);
	Enemy->SetActorRotation(NewRotation);

	return EStateTreeRunStatus::Running;
}
