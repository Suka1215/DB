// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/PartyMemberData.h"
#include "DBW_PartyList.generated.h"

class UVerticalBox;
class UDBW_PartyMember;

UCLASS()
class DAWNBLADE_API UDBW_PartyList : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitParty(const TArray<FPartyMemberData>& InPartyData);

protected:
	UPROPERTY(meta=(BindWidget)) UVerticalBox* PartyVBox;

	UPROPERTY(EditAnywhere, Category="DB|HUD")
	TSubclassOf<UDBW_PartyMember> PartyMemberClass;
};
