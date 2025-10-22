// MetaHumanStyleCharacterCreator.cpp
// AAA-ready UI implementation for MetaHuman Character Creator with enhanced visual polish and dynamic sizing

#include "MetaHumanStyleCharacterCreator.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Engine/GameViewportClient.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "FeaturePresetButton.h"
#include "ColorPickerButton.h"
#include "Animation/WidgetAnimation.h"
#include "Characters/Creation/CharacterCreatorComponent.h"
#include "Characters/Creation/CharacterCreatorDataAsset.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

// --------------------
// UCircularColorSwatch Implementation
// --------------------

void UCircularColorSwatch::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (SwatchButton)
    {
        SwatchButton->OnClicked.AddDynamic(this, &UCircularColorSwatch::OnSwatchClicked);
        SwatchButton->OnHovered.AddDynamic(this, &UCircularColorSwatch::OnSwatchHovered);
        SwatchButton->OnUnhovered.AddDynamic(this, &UCircularColorSwatch::OnSwatchUnhovered);
        SwatchButton->OnPressed.AddDynamic(this, &UCircularColorSwatch::OnSwatchPressed);
        SwatchButton->OnReleased.AddDynamic(this, &UCircularColorSwatch::OnSwatchReleased);
    }
}

void UCircularColorSwatch::NativeConstruct()
{
    Super::NativeConstruct();
    
    CreateCircularBrush();
    UpdateVisualState();
}

void UCircularColorSwatch::SetColorData(const FCharacterColorPreset& ColorPreset, int32 Index)
{
    ColorData = ColorPreset;
    ColorIndex = Index;
    
    CreateCircularBrush();
    UpdateVisualState();
}

void UCircularColorSwatch::SetSelected(bool bNewSelected)
{
    if (bIsSelected != bNewSelected)
    {
        bIsSelected = bNewSelected;
        UpdateVisualState();
        
        if (bIsSelected && SelectionAnimation)
        {
            PlayAnimation(SelectionAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.2f);
        }
    }
}

void UCircularColorSwatch::SetSwatchSize(float Size)
{
    SwatchSize = Size;
    CreateCircularBrush();
}

void UCircularColorSwatch::OnSwatchClicked()
{
    OnColorSwatchSelected.Broadcast(ColorIndex, ColorData.Color);
    
    if (PressAnimation)
    {
        PlayAnimation(PressAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.5f);
    }
}

void UCircularColorSwatch::OnSwatchHovered()
{
    if (HoverAnimation && !bIsSelected)
    {
        PlayAnimation(HoverAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
    }
}

void UCircularColorSwatch::OnSwatchUnhovered()
{
    if (HoverAnimation && !bIsSelected)
    {
        PlayAnimationReverse(HoverAnimation, 1.0f);
    }
}

void UCircularColorSwatch::OnSwatchPressed()
{
    if (ColorBorder)
    {
        ColorBorder->SetRenderScale(FVector2D(0.95f, 0.95f));
    }
}

void UCircularColorSwatch::OnSwatchReleased()
{
    if (ColorBorder)
    {
        ColorBorder->SetRenderScale(FVector2D(1.0f, 1.0f));
    }
}

void UCircularColorSwatch::UpdateVisualState()
{
    if (SelectionRing)
    {
        SelectionRing->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        
        if (bIsSelected)
        {
            FLinearColor RingColor = FLinearColor::White;
            RingColor.A = 0.9f;
            SelectionRing->SetBrushColor(RingColor);
        }
    }
}

void UCircularColorSwatch::CreateCircularBrush()
{
    if (!ColorBorder) return;

    const float BorderRadius = SwatchSize * 0.5f;
    
    // Create circular brush for the color
    FSlateBrush ColorBrush;
    ColorBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
    ColorBrush.OutlineSettings.CornerRadii = FVector4(BorderRadius, BorderRadius, BorderRadius, BorderRadius);
    ColorBrush.TintColor = ColorData.Color;
    ColorBrush.OutlineSettings.Width = 0.0f;
    ColorBrush.ImageSize = FVector2D(SwatchSize, SwatchSize);
    
    ColorBorder->SetBrush(ColorBrush);
    
    // Create selection ring brush
    if (SelectionRing)
    {
        const float RingSize = SwatchSize + 8.0f;
        const float RingRadius = RingSize * 0.5f;
        
        FSlateBrush RingBrush;
        RingBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
        RingBrush.OutlineSettings.CornerRadii = FVector4(RingRadius, RingRadius, RingRadius, RingRadius);
        RingBrush.TintColor = FLinearColor::White;
        RingBrush.OutlineSettings.Width = 3.0f;
        RingBrush.OutlineSettings.Color = FLinearColor::White;
        RingBrush.ImageSize = FVector2D(RingSize, RingSize);
        
        SelectionRing->SetBrush(RingBrush);
    }
}

// --------------------
// FGenericPresetData Implementation
// --------------------

FGenericPresetData::FGenericPresetData()
{
    DisplayName = FText::GetEmpty();
    PreviewThumbnail = nullptr;
    UnlockLevel = 1;
    bRequiresPremium = false;
    Category = ECharacterFeatureCategory::Hair;
    PresetIndex = 0;
}

FGenericPresetData::FGenericPresetData(const FHairPreset& HairPreset, int32 Index)
{
    DisplayName = HairPreset.DisplayName;
    PreviewThumbnail = HairPreset.PreviewThumbnail;
    UnlockLevel = HairPreset.UnlockLevel;
    bRequiresPremium = HairPreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Hair;
    PresetIndex = Index;
}

FGenericPresetData::FGenericPresetData(const FEyePreset& EyePreset, int32 Index)
{
    DisplayName = EyePreset.DisplayName;
    PreviewThumbnail = EyePreset.PreviewThumbnail;
    UnlockLevel = EyePreset.UnlockLevel;
    bRequiresPremium = EyePreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Eyes;
    PresetIndex = Index;
}

FGenericPresetData::FGenericPresetData(const FNosePreset& NosePreset, int32 Index)
{
    DisplayName = NosePreset.DisplayName;
    PreviewThumbnail = NosePreset.PreviewThumbnail;
    UnlockLevel = NosePreset.UnlockLevel;
    bRequiresPremium = NosePreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Nose;
    PresetIndex = Index;
}

FGenericPresetData::FGenericPresetData(const FMouthPreset& MouthPreset, int32 Index)
{
    DisplayName = MouthPreset.DisplayName;
    PreviewThumbnail = MouthPreset.PreviewThumbnail;
    UnlockLevel = MouthPreset.UnlockLevel;
    bRequiresPremium = MouthPreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Mouth;
    PresetIndex = Index;
}

FGenericPresetData::FGenericPresetData(const FBodyPreset& BodyPreset, int32 Index)
{
    DisplayName = BodyPreset.DisplayName;
    PreviewThumbnail = BodyPreset.PreviewThumbnail;
    UnlockLevel = BodyPreset.UnlockLevel;
    bRequiresPremium = BodyPreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Body;
    PresetIndex = Index;
}

FGenericPresetData::FGenericPresetData(const FMakeupPreset& MakeupPreset, int32 Index)
{
    DisplayName = MakeupPreset.DisplayName;
    PreviewThumbnail = MakeupPreset.PreviewThumbnail;
    UnlockLevel = MakeupPreset.UnlockLevel;
    bRequiresPremium = MakeupPreset.bRequiresPremium;
    Category = ECharacterFeatureCategory::Makeup;
    PresetIndex = Index;
}

// --------------------
// Dynamic Sizing Implementation
// --------------------

FVector2D UMetaHumanCharacterCreatorWidget::GetBreakpointThumbnailSize() const
{
    if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
    {
        FVector2D ViewportSize;
        ViewportClient->GetViewportSize(ViewportSize);
        
        // Find appropriate breakpoint
        for (const FThumbnailSizeBreakpoint& Breakpoint : SizeBreakpoints)
        {
            if (ViewportSize.X >= Breakpoint.MinScreenWidth)
            {
                UE_LOG(LogTemp, Log, TEXT("Using breakpoint for screen width %.0f: %s"), 
                    ViewportSize.X, *Breakpoint.ThumbnailSize.ToString());
                return Breakpoint.ThumbnailSize;
            }
        }
    }
    
    // Fallback to smallest size
    return SizeBreakpoints.Last().ThumbnailSize;
}

FVector2D UMetaHumanCharacterCreatorWidget::GetScaledThumbnailSize() const
{
    if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
    {
        FVector2D ViewportSize;
        ViewportClient->GetViewportSize(ViewportSize);
        
        // Calculate scale factor based on resolution
        float ScaleX = ViewportSize.X / ReferenceResolution.X;
        float ScaleY = ViewportSize.Y / ReferenceResolution.Y;
        float ScaleFactor = FMath::Min(ScaleX, ScaleY); // Use smaller scale to maintain aspect ratio
        
        // Clamp to min/max bounds
        ScaleFactor = FMath::Clamp(ScaleFactor, MinScaleFactor, MaxScaleFactor);
        
        FVector2D DynamicSize = BaseThumbnailSize * ScaleFactor;
        
        UE_LOG(LogTemp, Log, TEXT("Scaled Thumbnail Size: %s (Scale: %.2f)"), 
            *DynamicSize.ToString(), ScaleFactor);
            
        return DynamicSize;
    }
    
    return BaseThumbnailSize; // Fallback
}

FVector2D UMetaHumanCharacterCreatorWidget::GetCurrentThumbnailSize() const
{
    return bUseBreakpointSizing ? GetBreakpointThumbnailSize() : GetScaledThumbnailSize();
}

void UMetaHumanCharacterCreatorWidget::UpdateThumbnailSizes()
{
    FVector2D NewSize = GetCurrentThumbnailSize();

    for (UFeaturePresetButton* Button : FeaturePresetButtons)
    {
        if (Button)
        {
            Button->ApplyThumbnailSize(NewSize);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Updated %d preset thumbnails to %s"),
        FeaturePresetButtons.Num(), *NewSize.ToString());
}

void UMetaHumanCharacterCreatorWidget::OnViewportResized(FViewport* Viewport, uint32 ResizeCode)
{
    // Update thumbnail sizes when screen is resized
    UE_LOG(LogTemp, Log, TEXT("Viewport resized, updating thumbnail sizes"));
    UpdateThumbnailSizes();
}

// --------------------
// UMetaHumanCharacterCreatorWidget Implementation
// --------------------

UMetaHumanCharacterCreatorWidget::UMetaHumanCharacterCreatorWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UMetaHumanCharacterCreatorWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    BindButtons();
}

// In MetaHumanStyleCharacterCreator.cpp

void UMetaHumanCharacterCreatorWidget::LogButtonState(UFeaturePresetButton* Button, const FString& Context)
{
    if (!Button || !Button->ThumbnailImage) return;

    FString ButtonName = Button->Label_DisplayName ? Button->Label_DisplayName->GetText().ToString() : "UNKNOWN";
    FString StoredThumbnailName = Button->GetPreviewThumbnail() ? Button->GetPreviewThumbnail()->GetName() : "NULL";
    
    const FSlateBrush& CurrentBrush = Button->ThumbnailImage->GetBrush();
    FString BrushThumbnailName = CurrentBrush.GetResourceObject() ? CurrentBrush.GetResourceObject()->GetName() : "NULL";
    
    FString VisibilityString;
    switch (Button->ThumbnailImage->GetVisibility())
    {
    case ESlateVisibility::Visible: VisibilityString = "Visible"; break;
    case ESlateVisibility::Hidden: VisibilityString = "Hidden"; break;
    case ESlateVisibility::Collapsed: VisibilityString = "Collapsed"; break;
    default: VisibilityString = "Unknown";
    }

    UE_LOG(LogTemp, Warning, TEXT("[%s] Button '%s' State: StoredThumb='%s' | BrushThumb='%s' | Visibility='%s'"),
        *Context, *ButtonName, *StoredThumbnailName, *BrushThumbnailName, *VisibilityString);
}

void UMetaHumanCharacterCreatorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Hair)
    {
        Button_Hair->SetFocus();
    }

    SetActiveCategory(ECharacterFeatureCategory::Hair);
    UpdateSliderLabels();
    UpdateColorSwatchStyling();
    
    // Set initial thumbnail sizes
    UpdateThumbnailSizes();
}

void UMetaHumanCharacterCreatorWidget::NativeDestruct()
{
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->OnCharacterAppearanceChanged.RemoveDynamic(
            this, &UMetaHumanCharacterCreatorWidget::OnCharacterAppearanceChanged);
    }

    Super::NativeDestruct();
}

void UMetaHumanCharacterCreatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // Handle staggered color swatch animations
    if (bIsAnimatingColorGrid)
    {
        ColorGridAnimationTimer += InDeltaTime;
        
        // Check if animation is complete
        if (ColorGridAnimationTimer >= (SwatchAnimationDelays.Num() > 0 ? SwatchAnimationDelays.Last() + TransitionDuration : TransitionDuration))
        {
            bIsAnimatingColorGrid = false;
            ColorGridAnimationTimer = 0.0f;
        }
    }
}

UWidget* UMetaHumanCharacterCreatorWidget::NativeGetDesiredFocusTarget() const
{
    return Button_Hair;
}

TArray<FGenericPresetData> UMetaHumanCharacterCreatorWidget::GetPresetsForCategory(ECharacterFeatureCategory Category)
{
    TArray<FGenericPresetData> GenericPresets;

    if (!CreatorDataAsset)
    {
        return GenericPresets;
    }

    switch (Category)
    {
        case ECharacterFeatureCategory::Hair:
            for (int32 i = 0; i < CreatorDataAsset->GetHairPresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetHairPresets()[i], i));
            }
            break;
        case ECharacterFeatureCategory::Eyes:
            for (int32 i = 0; i < CreatorDataAsset->GetEyePresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetEyePresets()[i], i));
            }
            break;
        case ECharacterFeatureCategory::Nose:
            for (int32 i = 0; i < CreatorDataAsset->GetNosePresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetNosePresets()[i], i));
            }
            break;
        case ECharacterFeatureCategory::Mouth:
            for (int32 i = 0; i < CreatorDataAsset->GetMouthPresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetMouthPresets()[i], i));
            }
            break;
        case ECharacterFeatureCategory::Body:
            for (int32 i = 0; i < CreatorDataAsset->GetBodyPresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetBodyPresets()[i], i));
            }
            break;
        case ECharacterFeatureCategory::Makeup:
            for (int32 i = 0; i < CreatorDataAsset->GetMakeupPresets().Num(); ++i)
            {
                GenericPresets.Add(FGenericPresetData(CreatorDataAsset->GetMakeupPresets()[i], i));
            }
            break;
    }

    return GenericPresets;
}

void UMetaHumanCharacterCreatorWidget::SetCharacterCreatorComponent(UCharacterCreatorComponent* InComponent)
{
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->OnCharacterAppearanceChanged.RemoveDynamic(
            this, &UMetaHumanCharacterCreatorWidget::OnCharacterAppearanceChanged);
    }

    CharacterCreatorComponent = InComponent;

    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->OnCharacterAppearanceChanged.AddDynamic(
            this, &UMetaHumanCharacterCreatorWidget::OnCharacterAppearanceChanged);
        CreatorDataAsset = CharacterCreatorComponent->CreatorDataAsset;

        SetActiveCategory(CurrentCategory);
    }
}

void UMetaHumanCharacterCreatorWidget::SetActiveCategory(ECharacterFeatureCategory Category)
{
    CurrentCategory = Category;
    
    UpdateCategoryButtons();
    UpdateCategoryButtonStyling();

    // --- ADD THIS LINE ---
    // Set the default selection to the first preset in the category.
    CurrentFeaturePresetIndex = 0; 
    
    PopulateFeaturePresets();
    AnimateColorGridTransition();
    UpdatePropertySliders();

    if (CategorySwitchAnimation)
    {
        PlayAnimation(CategorySwitchAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
    }

    if (Text_ColorTitle)
    {
        FString CategoryDisplayName;
        switch (Category)
        {
            case ECharacterFeatureCategory::Hair:   CategoryDisplayName = TEXT("Hair Color"); break;
            case ECharacterFeatureCategory::Eyes:   CategoryDisplayName = TEXT("Eye Color"); break;
            case ECharacterFeatureCategory::Nose:   CategoryDisplayName = TEXT("Skin Tone"); break;
            case ECharacterFeatureCategory::Mouth:  CategoryDisplayName = TEXT("Lip Color"); break;
            case ECharacterFeatureCategory::Body:   CategoryDisplayName = TEXT("Skin Tone"); break;
            case ECharacterFeatureCategory::Makeup: CategoryDisplayName = TEXT("Makeup Color"); break;
        }
        Text_ColorTitle->SetText(FText::FromString(CategoryDisplayName));
    }
}

void UMetaHumanCharacterCreatorWidget::SelectFeaturePreset(int32 PresetIndex)
{
    CurrentFeaturePresetIndex = PresetIndex;
    UpdatePresetSelection();
    ApplyCurrentSettingsToCharacter();

    if (PresetSelectionAnimation)
    {
        PlayAnimation(PresetSelectionAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.2f);
    }
}

void UMetaHumanCharacterCreatorWidget::SetPrimaryColor(FLinearColor Color)
{
    CurrentPrimaryColor = Color;
    UpdateColorSelection();
    ApplyCurrentSettingsToCharacter();
}

void UMetaHumanCharacterCreatorWidget::SetSecondaryColor(FLinearColor Color)
{
    CurrentSecondaryColor = Color;
    UpdateColorSelection();
    ApplyCurrentSettingsToCharacter();
}

void UMetaHumanCharacterCreatorWidget::PlayColorGridIntroAnimation()
{
    CreateStaggeredColorSwatchAnimation();
}

void UMetaHumanCharacterCreatorWidget::UpdateColorSwatchStyling()
{
    for (UCircularColorSwatch* Swatch : CircularColorSwatches)
    {
        if (Swatch)
        {
            Swatch->SetSwatchSize(ColorSwatchSize);
        }
    }
}

// --------------------
// Button Callbacks
// --------------------
void UMetaHumanCharacterCreatorWidget::OnHairButtonClicked()    { SetActiveCategory(ECharacterFeatureCategory::Hair); }
void UMetaHumanCharacterCreatorWidget::OnEyesButtonClicked()    { SetActiveCategory(ECharacterFeatureCategory::Eyes); }
void UMetaHumanCharacterCreatorWidget::OnNoseButtonClicked()    { SetActiveCategory(ECharacterFeatureCategory::Nose); }
void UMetaHumanCharacterCreatorWidget::OnMouthButtonClicked()   { SetActiveCategory(ECharacterFeatureCategory::Mouth); }
void UMetaHumanCharacterCreatorWidget::OnBodyButtonClicked()    { SetActiveCategory(ECharacterFeatureCategory::Body); }
void UMetaHumanCharacterCreatorWidget::OnMakeupButtonClicked()  { SetActiveCategory(ECharacterFeatureCategory::Makeup); }

void UMetaHumanCharacterCreatorWidget::OnFeaturePresetSelected(int32 PresetIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("=== PRESET CLICKED ==="));
    UE_LOG(LogTemp, Warning, TEXT("Clicked PresetIndex: %d"), PresetIndex);
    UE_LOG(LogTemp, Warning, TEXT("Current Category: %d"), (int32)CurrentCategory);
    UE_LOG(LogTemp, Warning, TEXT("Before - CurrentFeaturePresetIndex: %d"), CurrentFeaturePresetIndex);
    
    SelectFeaturePreset(PresetIndex);
    
    UE_LOG(LogTemp, Warning, TEXT("After - CurrentFeaturePresetIndex: %d"), CurrentFeaturePresetIndex);
    UE_LOG(LogTemp, Warning, TEXT("=== PRESET CLICK END ==="));
}

void UMetaHumanCharacterCreatorWidget::OnColorSelected(int32 ColorIndex, bool bIsPrimaryColor)
{
    if (!CreatorDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnColorSelected called but CreatorDataAsset is NULL"));
        return;
    }

    TArray<FCharacterColorPreset> Colors = CreatorDataAsset->GetColorsForCategory(CurrentCategory);

    UE_LOG(LogTemp, Log, TEXT("OnColorSelected called: Index=%d, IsPrimary=%d, TotalColors=%d, Category=%d"),
        ColorIndex, bIsPrimaryColor ? 1 : 0, Colors.Num(), (int32)CurrentCategory);

    if (ColorIndex >= 0 && ColorIndex < Colors.Num())
    {
        const FLinearColor& SelectedColor = Colors[ColorIndex].Color;

        UE_LOG(LogTemp, Log, TEXT("Selected Color: %s (Index=%d, Primary=%d)"),
            *SelectedColor.ToString(), ColorIndex, bIsPrimaryColor ? 1 : 0);

        if (bIsPrimaryColor)
        {
            SetPrimaryColor(SelectedColor);
        }
        else
        {
            SetSecondaryColor(SelectedColor);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnColorSelected: Invalid ColorIndex %d (Total=%d)"),
            ColorIndex, Colors.Num());
    }
}

void UMetaHumanCharacterCreatorWidget::OnColorWheelClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Color wheel clicked - implement custom color picker"));
}

void UMetaHumanCharacterCreatorWidget::OnShadeChanged(float Value)
{
    if (CharacterCreatorComponent)
    {
        FCharacterAppearanceData CurrentData = CharacterCreatorComponent->GetCurrentAppearance();
        CurrentData.HairShade = Value;
        CharacterCreatorComponent->ApplyAppearance(CurrentData);
    }
    UpdateSliderLabels();
}

void UMetaHumanCharacterCreatorWidget::OnTransparentChanged(float Value)
{
    if (CharacterCreatorComponent)
    {
        FCharacterAppearanceData CurrentData = CharacterCreatorComponent->GetCurrentAppearance();
        CurrentData.HairTransparency = Value;
        CharacterCreatorComponent->ApplyAppearance(CurrentData);
    }
    UpdateSliderLabels();
}

void UMetaHumanCharacterCreatorWidget::OnTextureDetailChanged(float Value)
{
    if (CharacterCreatorComponent)
    {
        FCharacterAppearanceData CurrentData = CharacterCreatorComponent->GetCurrentAppearance();
        CurrentData.HairTextureDetail = Value;
        CharacterCreatorComponent->ApplyAppearance(CurrentData);
    }
    UpdateSliderLabels();
}

void UMetaHumanCharacterCreatorWidget::OnShareClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Share character"));
}

void UMetaHumanCharacterCreatorWidget::OnSaveClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Save character"));
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->SaveCharacterToDatabase(12345);
    }
}

void UMetaHumanCharacterCreatorWidget::OnResetClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Reset character"));
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->ResetToDefault();
    }
}

void UMetaHumanCharacterCreatorWidget::OnRetraceClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Retrace character"));
}

void UMetaHumanCharacterCreatorWidget::OnRestoreClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Restore character"));
}

void UMetaHumanCharacterCreatorWidget::OnCreateCharacterClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Create character - finalizing"));
    if (CharacterCreatorComponent)
    {
        CharacterCreatorComponent->SaveCharacterToDatabase(12345);
    }
}

void UMetaHumanCharacterCreatorWidget::OnCharacterAppearanceChanged(const FCharacterAppearanceData& AppearanceData)
{
    UpdatePropertySliders();
    UpdateSliderLabels();

    CurrentPrimaryColor = AppearanceData.HairColor;
    UpdateColorSelection();
}

void UMetaHumanCharacterCreatorWidget::OnColorPickerSelected(int32 ColorIndex, FLinearColor Color)
{
    OnColorSelected(ColorIndex, true);
}

void UMetaHumanCharacterCreatorWidget::OnPrimaryColorPickerSelected(int32 ColorIndex, FLinearColor Color)
{
    OnColorSelected(ColorIndex, true);
}

void UMetaHumanCharacterCreatorWidget::OnSecondaryColorPickerSelected(int32 ColorIndex, FLinearColor Color)
{
    OnColorSelected(ColorIndex, false);
}

void UMetaHumanCharacterCreatorWidget::OnCircularSwatchSelected(int32 ColorIndex, FLinearColor Color)
{
    OnColorSelected(ColorIndex, true);
}

// --------------------
// Internals
// --------------------
void UMetaHumanCharacterCreatorWidget::BindButtons()
{
    if (Button_Hair)      Button_Hair->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnHairButtonClicked);
    if (Button_Eyes)      Button_Eyes->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnEyesButtonClicked);
    if (Button_Nose)      Button_Nose->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnNoseButtonClicked);
    if (Button_Mouth)     Button_Mouth->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnMouthButtonClicked);
    if (Button_Body)      Button_Body->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnBodyButtonClicked);
    if (Button_Makeup)    Button_Makeup->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnMakeupButtonClicked);
    if (Button_ColorWheel) Button_ColorWheel->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnColorWheelClicked);

    if (Slider_Shade)        Slider_Shade->OnValueChanged.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnShadeChanged);
    if (Slider_Transparent)  Slider_Transparent->OnValueChanged.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnTransparentChanged);
    if (Slider_TextureDetail)Slider_TextureDetail->OnValueChanged.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnTextureDetailChanged);

    if (Button_Share)        Button_Share->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnShareClicked);
    if (Button_Save)         Button_Save->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnSaveClicked);
    if (Button_Reset)        Button_Reset->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnResetClicked);
    if (Button_Retrace)      Button_Retrace->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnRetraceClicked);
    if (Button_Restore)      Button_Restore->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnRestoreClicked);
    if (Button_CreateCharacter) Button_CreateCharacter->OnClicked().AddUObject(this, &UMetaHumanCharacterCreatorWidget::OnCreateCharacterClicked);
}

void UMetaHumanCharacterCreatorWidget::UpdateCategoryButtons()
{
    CategoryButtons = {Button_Hair, Button_Eyes, Button_Nose, Button_Mouth, Button_Body, Button_Makeup};

    for (int32 i = 0; i < CategoryButtons.Num(); ++i)
    {
        if (CategoryButtons[i])
        {
            bool bIsSelected = (static_cast<ECharacterFeatureCategory>(i) == CurrentCategory);
            CategoryButtons[i]->SetIsSelected(bIsSelected);
        }
    }
}

void UMetaHumanCharacterCreatorWidget::UpdateCategoryButtonStyling()
{
    for (int32 i = 0; i < CategoryButtons.Num(); ++i)
    {
        if (CategoryButtons[i])
        {
            bool bIsSelected = (static_cast<ECharacterFeatureCategory>(i) == CurrentCategory);
            
            if (bIsSelected)
            {
                CategoryButtons[i]->SetRenderScale(FVector2D(1.05f, 1.05f));
                CategoryButtons[i]->SetRenderOpacity(1.0f);
            }
            else
            {
                CategoryButtons[i]->SetRenderScale(FVector2D(1.0f, 1.0f));
                CategoryButtons[i]->SetRenderOpacity(0.7f);
            }
        }
    }
}

void UMetaHumanCharacterCreatorWidget::PopulateFeaturePresets()
{
    if (!FeaturePresetsGrid || !CreatorDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("PopulateFeaturePresets: Missing FeaturePresetsGrid or CreatorDataAsset!"));
        return;
    }

    FeaturePresetsGrid->ClearChildren();
    FeaturePresetButtons.Empty();

    // CRITICAL: Set uniform grid properties for perfect 3-across layout
    FVector2D ThumbnailSize = GetCurrentThumbnailSize();
    FVector2D CellPadding = FVector2D(10.0f, 15.0f); // Spacing between cells
    
    // Force the grid to use uniform cell sizes
    FeaturePresetsGrid->SetSlotPadding(FMargin(CellPadding.X, CellPadding.Y));
    
    TArray<FGenericPresetData> Presets = GetPresetsForCategory(CurrentCategory);

    for (int32 i = 0; i < Presets.Num(); ++i)
    {
        CreateFeaturePresetButton(Presets[i], i);
    }
}

void UMetaHumanCharacterCreatorWidget::PopulateColorGrid()
{
    if (!ColorGrid || !CreatorDataAsset) return;

    ColorGrid->ClearChildren();
    ColorButtons.Empty();
    CircularColorSwatches.Empty();

    ColorGrid->SetSlotPadding(FMargin(ColorSwatchSpacing));

    TArray<FCharacterColorPreset> Colors = CreatorDataAsset->GetColorsForCategory(CurrentCategory);

    for (int32 i = 0; i < Colors.Num(); ++i)
    {
        if (CircularColorSwatchClass)
        {
            CreateCircularColorSwatch(Colors[i], i);
        }
        else
        {
            CreateColorButton(Colors[i], i, true);
        }
    }

    PlayColorGridIntroAnimation();
}

void UMetaHumanCharacterCreatorWidget::AnimateColorGridTransition()
{
    if (ColorGridFadeOut)
    {
        PlayAnimation(ColorGridFadeOut, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.5f);
    }

    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
    {
        PopulateColorGrid();
        
        if (ColorGridFadeIn)
        {
            PlayAnimation(ColorGridFadeIn, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.2f);
        }
    }, TransitionDuration * 0.5f, false);
}

void UMetaHumanCharacterCreatorWidget::CreateStaggeredColorSwatchAnimation()
{
    SwatchAnimationDelays.Empty();
    bIsAnimatingColorGrid = true;
    ColorGridAnimationTimer = 0.0f;

    for (int32 i = 0; i < CircularColorSwatches.Num(); ++i)
    {
        if (CircularColorSwatches[i])
        {
            float AnimationDelay = i * 0.05f;
            SwatchAnimationDelays.Add(AnimationDelay);

            CircularColorSwatches[i]->SetRenderOpacity(0.0f);
            CircularColorSwatches[i]->SetRenderScale(FVector2D(0.8f, 0.8f));

            FTimerHandle DelayHandle;
            GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, i]()
            {
                if (CircularColorSwatches.IsValidIndex(i) && CircularColorSwatches[i])
                {
                    CircularColorSwatches[i]->SetRenderOpacity(1.0f);
                    CircularColorSwatches[i]->SetRenderScale(FVector2D(1.0f, 1.0f));
                }
            }, AnimationDelay, false);
        }
    }
}

void UMetaHumanCharacterCreatorWidget::UpdatePropertySliders()
{
    if (!CharacterCreatorComponent) return;

    FCharacterAppearanceData CurrentData = CharacterCreatorComponent->GetCurrentAppearance();

    if (Slider_Shade)        Slider_Shade->SetValue(CurrentData.HairShade);
    if (Slider_Transparent)  Slider_Transparent->SetValue(CurrentData.HairTransparency);
    if (Slider_TextureDetail)Slider_TextureDetail->SetValue(CurrentData.HairTextureDetail);
}

void UMetaHumanCharacterCreatorWidget::UpdateSliderLabels()
{
    if (Text_ShadeValue && Slider_Shade)
    {
        int32 ShadePercent = FMath::RoundToInt(Slider_Shade->GetValue() * 100.0f);
        Text_ShadeValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), ShadePercent)));
    }

    if (Text_TransparentValue && Slider_Transparent)
    {
        int32 TransparentPercent = FMath::RoundToInt(Slider_Transparent->GetValue() * 100.0f);
        Text_TransparentValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), TransparentPercent)));
    }

    if (Text_TextureDetailValue && Slider_TextureDetail)
    {
        int32 DetailPercent = FMath::RoundToInt(Slider_TextureDetail->GetValue() * 100.0f);
        Text_TextureDetailValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), DetailPercent)));
    }
}

void UMetaHumanCharacterCreatorWidget::CreateFeaturePresetButton(const FGenericPresetData& Preset, int32 Index)
{
    if (!FeaturePresetsGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateFeaturePresetButton: FeaturePresetsGrid is null!"));
        return;
    }

    if (!FeaturePresetButtonClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateFeaturePresetButton: FeaturePresetButtonClass is NULL! Cannot create widget."));
        return;
    }

    UFeaturePresetButton* PresetButton = CreateWidget<UFeaturePresetButton>(this, FeaturePresetButtonClass);
    if (PresetButton)
    {
        // 1. Set the data on the button first. This runs its internal UpdateUI().
        PresetButton->SetPresetData(
            Preset.DisplayName,
            Preset.PreviewThumbnail,
            Preset.UnlockLevel,
            Preset.bRequiresPremium,
            Index
        );

        PresetButton->OnFeaturePresetSelected.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnFeaturePresetSelected);

        // --- CORRECTED LOGIC ---
        // 2. Now, apply dynamic sizing by creating a new brush that explicitly includes BOTH the texture and the size.
        // This ensures we don't lose the texture information.
        if (PresetButton->ThumbnailImage && Preset.PreviewThumbnail)
        {
            FVector2D DynamicSize = GetCurrentThumbnailSize();

            // Create a new brush from scratch
            FSlateBrush NewSizedBrush;
            NewSizedBrush.SetResourceObject(Preset.PreviewThumbnail); // Explicitly assign the texture
            NewSizedBrush.ImageSize = DynamicSize;                    // Set the desired size
            NewSizedBrush.DrawAs = ESlateBrushDrawType::Image;        // Ensure it draws as an image

            PresetButton->ThumbnailImage->SetBrush(NewSizedBrush);
            
            UE_LOG(LogTemp, Log, TEXT("Applied dynamic size %s to new preset button '%s'"), *DynamicSize.ToString(), *Preset.DisplayName.ToString());
        }

        int32 Row = Index / PresetsPerRow;
        int32 Column = Index % PresetsPerRow;

        if (UUniformGridSlot* GridSlot = ColorGrid->AddChildToUniformGrid(PresetButton, Row, Column))
        {
            GridSlot->SetHorizontalAlignment(HAlign_Center);
            GridSlot->SetVerticalAlignment(VAlign_Center);
        }

        FeaturePresetsGrid->AddChildToUniformGrid(PresetButton, Row, Column);
        FeaturePresetButtons.Add(PresetButton);

        LogButtonState(PresetButton, TEXT("A. After Creation"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CreateFeaturePresetButton: Failed to create widget!"));
    }
}

void UMetaHumanCharacterCreatorWidget::CreateColorButton(const FCharacterColorPreset& ColorPreset, int32 Index, bool bIsPrimaryColor)
{
    if (!ColorButtonClass || !ColorGrid) return;

    UColorPickerButton* ColorButton = CreateWidget<UColorPickerButton>(this, ColorButtonClass);
    if (ColorButton)
    {
        ColorButton->SetColorData(ColorPreset, Index);

        if (bIsPrimaryColor)
            ColorButton->OnColorPickerSelected.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnPrimaryColorPickerSelected);
        else
            ColorButton->OnColorPickerSelected.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnSecondaryColorPickerSelected);

        StyleColorButtonAsCircle(ColorButton, ColorPreset.Color);

        int32 Row = Index / ColorSwatchesPerRow;
        int32 Column = Index % ColorSwatchesPerRow;
        
        if (UUniformGridSlot* GridSlot = ColorGrid->AddChildToUniformGrid(ColorButton, Row, Column))
        {
            GridSlot->SetHorizontalAlignment(HAlign_Center);
            GridSlot->SetVerticalAlignment(VAlign_Center);
        }

        if (bIsPrimaryColor) ColorButtons.Add(ColorButton);
        else SecondaryColorButtons.Add(ColorButton);
    }
}

void UMetaHumanCharacterCreatorWidget::CreateCircularColorSwatch(const FCharacterColorPreset& ColorPreset, int32 Index)
{
    if (!CircularColorSwatchClass || !ColorGrid) return;

    UCircularColorSwatch* ColorSwatch = CreateWidget<UCircularColorSwatch>(this, CircularColorSwatchClass);
    if (ColorSwatch)
    {
        ColorSwatch->SetColorData(ColorPreset, Index);
        ColorSwatch->SetSwatchSize(ColorSwatchSize);
        
        ColorSwatch->OnColorSwatchSelected.AddDynamic(this, &UMetaHumanCharacterCreatorWidget::OnCircularSwatchSelected);

        int32 Row = Index / ColorSwatchesPerRow;
        int32 Column = Index % ColorSwatchesPerRow;
        
        if (UUniformGridSlot* GridSlot = ColorGrid->AddChildToUniformGrid(ColorSwatch, Row, Column))
        {
            GridSlot->SetHorizontalAlignment(HAlign_Center);
            GridSlot->SetVerticalAlignment(VAlign_Center);
        }

        CircularColorSwatches.Add(ColorSwatch);

        UE_LOG(LogTemp, Log, TEXT("Spawned CircularColorSwatch at Row=%d Col=%d, Color=%s"),
            Row, Column, *ColorPreset.Color.ToString());
    }
}

void UMetaHumanCharacterCreatorWidget::StyleColorButtonAsCircle(UColorPickerButton* ColorButton, const FLinearColor& Color)
{
    if (!ColorButton) return;

    ColorButton->SetRenderScale(FVector2D(1.0f, 1.0f));
    
    if (ColorButton->ColorBorder)
    {
        ColorButton->ColorBorder->SetBrushColor(Color);
        
        FSlateBrush CircleBrush;
        CircleBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
        CircleBrush.OutlineSettings.CornerRadii = FVector4(ColorSwatchSize * 0.5f, ColorSwatchSize * 0.5f, ColorSwatchSize * 0.5f, ColorSwatchSize * 0.5f);
        CircleBrush.TintColor = Color;
        CircleBrush.OutlineSettings.Width = 0.0f;
        CircleBrush.ImageSize = FVector2D(ColorSwatchSize, ColorSwatchSize);
        
        ColorButton->ColorBorder->SetBrush(CircleBrush);
    }
}

void UMetaHumanCharacterCreatorWidget::UpdatePresetSelection()
{
    for (int32 i = 0; i < FeaturePresetButtons.Num(); ++i)
    {
        if (FeaturePresetButtons[i])
        {
            FeaturePresetButtons[i]->SetPresetSelected(i == CurrentFeaturePresetIndex);
        }
    }
}

void UMetaHumanCharacterCreatorWidget::UpdateColorSelection()
{
    for (UCircularColorSwatch* ColorSwatch : CircularColorSwatches)
    {
        if (ColorSwatch)
        {
            FCharacterColorPreset SwatchColorData = ColorSwatch->GetColorData();
            bool bIsSelected =
                FMath::IsNearlyEqual(SwatchColorData.Color.R, CurrentPrimaryColor.R, 0.01f) &&
                FMath::IsNearlyEqual(SwatchColorData.Color.G, CurrentPrimaryColor.G, 0.01f) &&
                FMath::IsNearlyEqual(SwatchColorData.Color.B, CurrentPrimaryColor.B, 0.01f);
            ColorSwatch->SetSelected(bIsSelected);
        }
    }

    for (UColorPickerButton* ColorButton : ColorButtons)
    {
        if (ColorButton)
        {
            bool bIsSelected =
                FMath::IsNearlyEqual(ColorButton->ButtonColor.R, CurrentPrimaryColor.R, 0.01f) &&
                FMath::IsNearlyEqual(ColorButton->ButtonColor.G, CurrentPrimaryColor.G, 0.01f) &&
                FMath::IsNearlyEqual(ColorButton->ButtonColor.B, CurrentPrimaryColor.B, 0.01f);
            ColorButton->SetColorSelected(bIsSelected);
        }
    }
}

void UMetaHumanCharacterCreatorWidget::ApplyCurrentSettingsToCharacter()
{
    if (!CharacterCreatorComponent || !CreatorDataAsset) return;

    FCharacterAppearanceData CurrentData = CharacterCreatorComponent->GetCurrentAppearance();

    switch (CurrentCategory)
    {
        case ECharacterFeatureCategory::Hair:
            CurrentData.HairColor = CurrentPrimaryColor;
            CurrentData.HairPresetIndex = CurrentFeaturePresetIndex;
            break;
        case ECharacterFeatureCategory::Eyes:
            CurrentData.EyeColor = CurrentPrimaryColor;
            CurrentData.EyePresetIndex = CurrentFeaturePresetIndex;
            break;
        case ECharacterFeatureCategory::Nose:
            CurrentData.SkinTone = CurrentPrimaryColor;
            CurrentData.NosePresetIndex = CurrentFeaturePresetIndex;
            break;
        case ECharacterFeatureCategory::Mouth:
            CurrentData.LipColor = CurrentPrimaryColor;
            CurrentData.MouthPresetIndex = CurrentFeaturePresetIndex;
            break;
        case ECharacterFeatureCategory::Body:
            CurrentData.SkinTone = CurrentPrimaryColor;
            CurrentData.MeshSlots.BodyPresetIndex = CurrentFeaturePresetIndex;
            break;
        case ECharacterFeatureCategory::Makeup:
            CurrentData.MakeupColor = CurrentPrimaryColor;
            CurrentData.MakeupPresetIndex = CurrentFeaturePresetIndex;
            break;
    }

    CharacterCreatorComponent->ApplyAppearance(CurrentData);
}