// Fill out your copyright notice in the Description page of Project Settings.
#include "STTask_CheckHasWeapon.h"
#include "Components/DBEquipmentComponent.h"

EStateTreeRunStatus USTTask_CheckHasWeapon::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	bHasWeapon = false;

	if (!Enemy) return EStateTreeRunStatus::Succeeded;

	// Check equipment component for main hand weapon
	if (UDBEquipmentComponent* Equipment = Enemy->FindComponentByClass<UDBEquipmentComponent>())
	{
		// You'll need to implement GetMainHandWeapon() in your equipment component
		// This is a placeholder for now
		bHasWeapon = true; // Assume has weapon for now
        
		UE_LOG(LogTemp, Log, TEXT("🗡️ %s has weapon: %s"), 
			*Enemy->GetName(), 
			bHasWeapon ? TEXT("YES") : TEXT("NO"));
	}

	return EStateTreeRunStatus::Succeeded;
}