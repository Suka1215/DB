// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "GUI/HUD/DBW_PartyMember.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

void UDBW_PartyMember::InitPartyMember(const FPartyMemberData& Data)
{
	UpdateFromData(Data);
}

void UDBW_PartyMember::UpdateFromData(const FPartyMemberData& Data)
{
	if (NameText)
		NameText->SetText(FText::FromString(Data.Name));

	if (LevelText)
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Lv %d"), Data.Level)));

	if (HPBar)
		HPBar->SetPercent(Data.MaxHP > 0 ? Data.CurrentHP / Data.MaxHP : 0.f);

	if (MPBar)
		MPBar->SetPercent(Data.MaxMP > 0 ? Data.CurrentMP / Data.MaxMP : 0.f);

	if (ClassIcon && Data.JobIcon)
		ClassIcon->SetBrushFromTexture(Data.JobIcon);

	// Buffs
	if (Buffs_HBox)
	{
		Buffs_HBox->ClearChildren();
		for (UTexture2D* BuffTex : Data.ActiveBuffs)
		{
			if (!BuffTex) continue;
			UImage* BuffImage = NewObject<UImage>(this);
			BuffImage->SetBrushFromTexture(BuffTex);
			Buffs_HBox->AddChild(BuffImage);
		}
	}
}
