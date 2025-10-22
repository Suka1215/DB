#include "FeaturePresetButton.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "CommonTextBlock.h"

UFeaturePresetButton::UFeaturePresetButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsSelectable(true);
	SetIsFocusable(true);
}

void UFeaturePresetButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	OnClicked().AddUObject(this, &UFeaturePresetButton::HandleOnClicked);
}

void UFeaturePresetButton::NativeConstruct()
{
	Super::NativeConstruct();
	SetPresetSelected(bIsSelected);
}

void UFeaturePresetButton::SetPresetData(const FText& InDisplayName, UTexture2D* InThumbnail, int32 InUnlockLevel, bool bInRequiresPremium, int32 InPresetIndex)
{
	DisplayName = InDisplayName;
	PreviewThumbnail = InThumbnail;
	UnlockLevel = InUnlockLevel;
	bRequiresPremium = bInRequiresPremium;
	PresetIndex = InPresetIndex;

	UpdateUI();
}

void UFeaturePresetButton::UpdateUI()
{
	if (Label_DisplayName)
	{
		Label_DisplayName->SetText(DisplayName);
	}

	if (LockIcon)
	{
		LockIcon->SetVisibility(bRequiresPremium ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ThumbnailImage)
	{
		if (PreviewThumbnail)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(PreviewThumbnail);
			Brush.ImageSize = FVector2D(192, 256); // default, overridden later
			Brush.DrawAs = ESlateBrushDrawType::Image;

			ThumbnailImage->SetBrush(Brush);
			ThumbnailImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ThumbnailImage->SetBrushFromTexture(nullptr);
			ThumbnailImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UFeaturePresetButton::SetPresetSelected(bool bInIsSelected)
{
	bIsSelected = bInIsSelected;

	// if (SelectionBorder)
	// {
	// 	SelectionBorder->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	// }
}

void UFeaturePresetButton::ApplyThumbnailSize(const FVector2D& NewSize)
{
	if (SizeBox_Root)
	{
		// Fix the width, let height auto-adjust
		SizeBox_Root->SetWidthOverride(NewSize.X);
		SizeBox_Root->ClearHeightOverride(); // ← this is key
	}

	if (ThumbnailImage && PreviewThumbnail)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(PreviewThumbnail);

		// Set the width, but compute height dynamically from aspect ratio
		const float AspectRatio = (PreviewThumbnail->GetSizeY() > 0)
			? (static_cast<float>(PreviewThumbnail->GetSizeX()) / PreviewThumbnail->GetSizeY())
			: 1.0f;

		const float AutoHeight = NewSize.X / AspectRatio;
		Brush.ImageSize = FVector2D(NewSize.X, AutoHeight);

		Brush.DrawAs = ESlateBrushDrawType::Image;
		ThumbnailImage->SetBrush(Brush);
		ThumbnailImage->SetVisibility(ESlateVisibility::Visible);

		UE_LOG(LogTemp, Log, TEXT("Applied width=%f autoHeight=%f to preset '%s'"),
			NewSize.X, AutoHeight, *DisplayName.ToString());
	}
}

void UFeaturePresetButton::HandleOnClicked()
{
	OnFeaturePresetSelected.Broadcast(PresetIndex);
}
