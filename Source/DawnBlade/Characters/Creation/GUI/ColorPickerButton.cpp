#include "ColorPickerButton.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Engine/Engine.h"

UColorPickerButton::UColorPickerButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsSelectable(true);
    bIsFocusable = true;
}

void UColorPickerButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    OnClicked().AddUObject(this, &UColorPickerButton::HandleOnClicked);
}

void UColorPickerButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (ColorBorder)
    {
        FLinearColor SolidColor = ButtonColor;
        SolidColor.A = 1.0f; // Ensure visible
        ColorBorder->SetBrushColor(SolidColor);
    }

    if (LockIcon)
    {
        LockIcon->SetVisibility(bRequiresPremium ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (SelectionOutline)
    {
        SelectionOutline->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    SetColorSelected(bIsSelected);
}

void UColorPickerButton::SetColorData(const FCharacterColorPreset& InColorPreset, int32 InColorIndex)
{
    ButtonColor = InColorPreset.Color;
    UnlockLevel = InColorPreset.UnlockLevel;
    bRequiresPremium = InColorPreset.bRequiresPremium;
    ColorIndex = InColorIndex;

    if (ColorBorder)
    {
        ColorBorder->SetBrushColor(ButtonColor);
    }

    if (LockIcon)
    {
        LockIcon->SetVisibility(bRequiresPremium ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UColorPickerButton::SetColorSelected(bool bInSelected)
{
    bIsSelected = bInSelected;

    if (ColorBorder)
    {
        // Slightly brighten when selected
        ColorBorder->SetBrushColor(bIsSelected ? ButtonColor * 1.25f : ButtonColor);
    }

    if (SelectionOutline)
    {
        SelectionOutline->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UColorPickerButton::HandleOnClicked()
{
    OnColorPickerSelected.Broadcast(ColorIndex, ButtonColor);
}

void UColorPickerButton::NativeOnHovered()
{
    Super::NativeOnHovered();

    if (ColorBorder)
    {
        ColorBorder->SetBrushColor(ButtonColor * 1.15f);
    }

    // Scale up slightly for hover feedback
    SetRenderScale(FVector2D(1.1f, 1.1f));
}

void UColorPickerButton::NativeOnUnhovered()
{
    Super::NativeOnUnhovered();

    if (ColorBorder)
    {
        ColorBorder->SetBrushColor(bIsSelected ? ButtonColor * 1.25f : ButtonColor);
    }

    // Reset scale
    SetRenderScale(FVector2D(1.f, 1.f));
}