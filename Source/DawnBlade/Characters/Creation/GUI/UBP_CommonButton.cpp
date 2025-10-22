// Fill out your copyright notice in the Description page of Project Settings.

#include "UBP_CommonButton.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBP_CommonButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IconImage && Icon)
	{
		IconImage->SetBrushFromTexture(Icon);
		IconImage->SetBrushSize(FVector2D(IconWidth, IconHeight));
	}

	if (LabelText)
	{
		LabelText->SetText(Label);
	}
}
