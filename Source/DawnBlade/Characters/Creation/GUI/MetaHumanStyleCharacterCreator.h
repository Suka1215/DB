// MetaHumanStyleCharacterCreator.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridSlot.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/GameViewportClient.h"
#include "MetaHumanStyleCharacterCreator.generated.h"

// Forward declarations
class UCharacterCreatorDataAsset;
class UCharacterCreatorComponent;
class UFeaturePresetButton;
class UColorPickerButton;
class UCommonButtonBase;
class UCanvasPanel;
class UVerticalBox;
class UHorizontalBox;
class UCommonTextBlock;
class UUniformGridPanel;
class UScrollBox;
class USlider;
class UWidgetAnimation;

/**
 * Thumbnail size breakpoint for different screen sizes
 */
USTRUCT(BlueprintType)
struct DAWNBLADE_API FThumbnailSizeBreakpoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinScreenWidth = 1920.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D ThumbnailSize = FVector2D(256, 320);

    FThumbnailSizeBreakpoint() = default;
    
    FThumbnailSizeBreakpoint(float Width, FVector2D Size)
        : MinScreenWidth(Width), ThumbnailSize(Size) {}
};

/**
 * Enhanced circular color swatch widget with AAA polish
 */
UCLASS(BlueprintType, Blueprintable)
class DAWNBLADE_API UCircularColorSwatch : public UUserWidget
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorSwatchSelected, int32, ColorIndex, FLinearColor, Color);

    UPROPERTY(BlueprintAssignable)
    FOnColorSwatchSelected OnColorSwatchSelected;

    UFUNCTION(BlueprintCallable, Category = "Color Swatch")
    void SetColorData(const FCharacterColorPreset& ColorPreset, int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Color Swatch")
    void SetSelected(bool bIsSelected);

    UFUNCTION(BlueprintCallable, Category = "Color Swatch")
    void SetSwatchSize(float Size);

    UFUNCTION(BlueprintCallable, Category = "Color Swatch")
    FCharacterColorPreset GetColorData() const { return ColorData; }

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;

    // Widget components
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* SwatchButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* ColorBorder;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* SelectionRing;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* ColorImage;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HoverAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* SelectionAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* PressAnimation;

private:
    UFUNCTION()
    void OnSwatchClicked();

    UFUNCTION()
    void OnSwatchHovered();

    UFUNCTION()
    void OnSwatchUnhovered();

    UFUNCTION()
    void OnSwatchPressed();

    UFUNCTION()
    void OnSwatchReleased();

    FCharacterColorPreset ColorData;
    int32 ColorIndex = -1;
    bool bIsSelected = false;
    float SwatchSize = 80.0f;

    void UpdateVisualState();
    void CreateCircularBrush();
};

/**
 * Generic wrapper for any preset type.
 */
USTRUCT(BlueprintType)
struct DAWNBLADE_API FGenericPresetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    TObjectPtr<UTexture2D> PreviewThumbnail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 UnlockLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    bool bRequiresPremium = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category")
    ECharacterFeatureCategory Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Category")
    int32 PresetIndex = 0;

    FGenericPresetData();
    FGenericPresetData(const FHairPreset& HairPreset, int32 Index);
    FGenericPresetData(const FEyePreset& EyePreset, int32 Index);
    FGenericPresetData(const FNosePreset& NosePreset, int32 Index);
    FGenericPresetData(const FMouthPreset& MouthPreset, int32 Index);
    FGenericPresetData(const FBodyPreset& BodyPreset, int32 Index);
    FGenericPresetData(const FMakeupPreset& MakeupPreset, int32 Index);
};

/**
 * Main widget for MetaHuman Character Creator UI with dynamic sizing support
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DAWNBLADE_API UMetaHumanCharacterCreatorWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UMetaHumanCharacterCreatorWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void OnViewportResized(FViewport* Viewport, uint32 ResizeCode);

    TArray<FGenericPresetData> GetPresetsForCategory(ECharacterFeatureCategory Category);

public:
    // === Widget Components ===
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCanvasPanel* RootCanvas;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* LeftSidePanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Hair;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Eyes;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Nose;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Mouth;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Body;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Makeup;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UScrollBox* FeaturePresetsScroll;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UUniformGridPanel* FeaturePresetsGrid;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* RightSidePanel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonTextBlock* Text_ColorTitle;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UUniformGridPanel* ColorGrid;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_ColorWheel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* PropertySlidersContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    USlider* Slider_Shade;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonTextBlock* Text_ShadeValue;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    USlider* Slider_Transparent;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonTextBlock* Text_TransparentValue;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    USlider* Slider_TextureDetail;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonTextBlock* Text_TextureDetailValue;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UHorizontalBox* TopActionBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Share;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Save;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Reset;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Retrace;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_Restore;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCommonButtonBase* Button_CreateCharacter;

    // === Configuration ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
    UCharacterCreatorDataAsset* CreatorDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Classes")
    TSubclassOf<UFeaturePresetButton> FeaturePresetButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Classes")
    TSubclassOf<UColorPickerButton> ColorButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Classes")
    TSubclassOf<UCircularColorSwatch> CircularColorSwatchClass;

    // === Dynamic Sizing Configuration ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    TArray<FThumbnailSizeBreakpoint> SizeBreakpoints = {
        FThumbnailSizeBreakpoint(2560.0f, FVector2D(256, 342)), // 4K screens
        FThumbnailSizeBreakpoint(1920.0f, FVector2D(192, 256)), // 1080p screens  
        FThumbnailSizeBreakpoint(1600.0f, FVector2D(160, 213)), // 1600p screens
        FThumbnailSizeBreakpoint(1366.0f, FVector2D(144, 192)), // 1366p screens
        FThumbnailSizeBreakpoint(1280.0f, FVector2D(128, 171)), // 1280p screens
        FThumbnailSizeBreakpoint(0.0f,    FVector2D(96, 128))   // Mobile/small screens
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    bool bUseBreakpointSizing = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    FVector2D BaseThumbnailSize = FVector2D(192, 256);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    FVector2D ReferenceResolution = FVector2D(1920, 1080);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    float MinScaleFactor = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Sizing")
    float MaxScaleFactor = 2.0f;

    // === Visual Configuration ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    float ColorSwatchSize = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    float ColorSwatchSpacing = 16.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    int32 ColorSwatchesPerRow = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    int32 PresetsPerRow = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    float TransitionDuration = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    FLinearColor SelectionRingColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Settings")
    float SelectionRingThickness = 3.0f;

    // === Current State ===
    UPROPERTY(BlueprintReadOnly, Category = "State")
    UCharacterCreatorComponent* CharacterCreatorComponent;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    ECharacterFeatureCategory CurrentCategory = ECharacterFeatureCategory::Hair;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    int32 CurrentFeaturePresetIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    FLinearColor CurrentPrimaryColor = FLinearColor::Black;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    FLinearColor CurrentSecondaryColor = FLinearColor::White;

    // === Public API ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetCharacterCreatorComponent(UCharacterCreatorComponent* InComponent);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetActiveCategory(ECharacterFeatureCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SelectFeaturePreset(int32 PresetIndex);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetPrimaryColor(FLinearColor Color);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetSecondaryColor(FLinearColor Color);

    UFUNCTION(BlueprintCallable, Category = "Visual Polish")
    void PlayColorGridIntroAnimation();

    UFUNCTION(BlueprintCallable, Category = "Visual Polish")
    void UpdateColorSwatchStyling();

    // === Dynamic Sizing API ===
    UFUNCTION(BlueprintCallable, Category = "Dynamic Sizing")
    FVector2D GetCurrentThumbnailSize() const;

    UFUNCTION(BlueprintCallable, Category = "Dynamic Sizing")
    void UpdateThumbnailSizes();

protected:
    // === Dynamic Sizing Functions ===
    FVector2D GetBreakpointThumbnailSize() const;
    FVector2D GetScaledThumbnailSize() const;

    // === Callbacks ===
    UFUNCTION() void OnHairButtonClicked();
    UFUNCTION() void OnEyesButtonClicked();
    UFUNCTION() void OnNoseButtonClicked();
    UFUNCTION() void OnMouthButtonClicked();
    UFUNCTION() void OnBodyButtonClicked();
    UFUNCTION() void OnMakeupButtonClicked();
    UFUNCTION() void OnFeaturePresetSelected(int32 PresetIndex);
    UFUNCTION() void OnColorSelected(int32 ColorIndex, bool bIsPrimaryColor);
    UFUNCTION() void OnColorWheelClicked();
    UFUNCTION() void OnShadeChanged(float Value);
    UFUNCTION() void OnTransparentChanged(float Value);
    UFUNCTION() void OnTextureDetailChanged(float Value);
    UFUNCTION() void OnShareClicked();
    UFUNCTION() void OnSaveClicked();
    UFUNCTION() void OnResetClicked();
    UFUNCTION() void OnRetraceClicked();
    UFUNCTION() void OnRestoreClicked();
    UFUNCTION() void OnCreateCharacterClicked();
    UFUNCTION() void OnCharacterAppearanceChanged(const FCharacterAppearanceData& AppearanceData);
    UFUNCTION() void OnColorPickerSelected(int32 ColorIndex, FLinearColor Color);
    UFUNCTION() void OnPrimaryColorPickerSelected(int32 ColorIndex, FLinearColor Color);
    UFUNCTION() void OnSecondaryColorPickerSelected(int32 ColorIndex, FLinearColor Color);
    UFUNCTION() void OnCircularSwatchSelected(int32 ColorIndex, FLinearColor Color);

private:
    // === Internal Functions ===
    void LogButtonState(UFeaturePresetButton* Button, const FString& Context);

    void BindButtons();
    void UpdateCategoryButtons();
    void PopulateFeaturePresets();
    void PopulateColorGrid();
    void UpdatePropertySliders();
    void UpdateSliderLabels();
    void CreateFeaturePresetButton(const FGenericPresetData& Preset, int32 Index);
    void CreateColorButton(const FCharacterColorPreset& ColorPreset, int32 Index, bool bIsPrimaryColor);
    void CreateCircularColorSwatch(const FCharacterColorPreset& ColorPreset, int32 Index);
    void UpdatePresetSelection();
    void UpdateColorSelection();
    void ApplyCurrentSettingsToCharacter();

    // === Enhanced Visual Functions ===
    void StyleColorButtonAsCircle(UColorPickerButton* ColorButton, const FLinearColor& Color);
    void AnimateColorGridTransition();
    void UpdateCategoryButtonStyling();
    void CreateStaggeredColorSwatchAnimation();

    // === Widget Storage ===
    UPROPERTY() TArray<UCommonButtonBase*> CategoryButtons;
    UPROPERTY() TArray<UFeaturePresetButton*> FeaturePresetButtons;
    UPROPERTY() TArray<UColorPickerButton*> ColorButtons;
    UPROPERTY() TArray<UColorPickerButton*> SecondaryColorButtons;
    UPROPERTY() TArray<UCircularColorSwatch*> CircularColorSwatches;

    // === Animation Support ===
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* CategorySwitchAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* PresetSelectionAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ColorGridFadeIn;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ColorGridFadeOut;

    // === Animation State ===
    bool bIsAnimatingColorGrid = false;
    float ColorGridAnimationTimer = 0.0f;
    TArray<float> SwatchAnimationDelays;
};