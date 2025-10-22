// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GUI/HUD/DBW_PartyList.h"
#include "GUI/HUD/DBW_PartyMember.h"
#include "Components/VerticalBox.h"

void UDBW_PartyList::InitParty(const TArray<FPartyMemberData>& InPartyData)
{
	if (!PartyVBox || !PartyMemberClass) return;

	PartyVBox->ClearChildren();

	for (const FPartyMemberData& MemberData : InPartyData)
	{
		if (MemberData.bIsLocalPlayer)
			continue; // skip self

		UDBW_PartyMember* Entry = CreateWidget<UDBW_PartyMember>(GetWorld(), PartyMemberClass);
		if (Entry)
		{
			Entry->InitPartyMember(MemberData);
			PartyVBox->AddChild(Entry);
		}
	}
}

