// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DBHUD.generated.h"

/**
 * 
 */
UCLASS()
class DAWNBLADE_API ADBHUD : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) void ShowCombatPalette(bool bShow);
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<class UUserWidget> CombatPaletteClass;
	UPROPERTY() class UUserWidget* CombatPaletteWidget = nullptr;
};