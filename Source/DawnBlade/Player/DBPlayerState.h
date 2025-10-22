// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DBAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "DBPlayerState.generated.h"


class UDBAbilitySystemComponent;
class UDBAttr_Stats;
class UDBAttr_Combat;

/**
 * 
 */
UCLASS()
class DAWNBLADE_API ADBPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ADBPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UDBAbilitySystemComponent* ASC;
	UPROPERTY() UDBAttr_Stats*  AttrStats;
	UPROPERTY() UDBAttr_Combat* AttrCombat;

	UPROPERTY(EditDefaultsOnly, Category="GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_InitStats;

	UPROPERTY(EditDefaultsOnly, Category="GAS|Startup")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	void InitASCForPawn(APawn* NewPawn);

protected:
	virtual void BeginPlay() override;

private:
	void ApplyInitStats();
	void GrantStartupAbilities();
};
