#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "ColorPickerButton.generated.h"

// Delegate when a color is selected
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorPickerSelectedSignature, int32, ColorIndex, FLinearColor, Color);

UCLASS()
class DAWNBLADE_API UColorPickerButton : public UCommonButtonBase
{
    GENERATED_BODY()

public:
    UColorPickerButton(const FObjectInitializer& ObjectInitializer);

    // === Public API ===
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetColorData(const FCharacterColorPreset& InColorPreset, int32 InColorIndex);

    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void SetColorSelected(bool bInSelected);

    // Fired when clicked
    UPROPERTY(BlueprintAssignable, Category = "Character Creator")
    FOnColorPickerSelectedSignature OnColorPickerSelected;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;

    virtual void NativeOnHovered() override;
    virtual void NativeOnUnhovered() override;

    UFUNCTION()
    void HandleOnClicked();

public:
    // === Data ===
    UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
    int32 ColorIndex = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
    FLinearColor ButtonColor = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
    bool bRequiresPremium = false;

    UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
    int32 UnlockLevel = 1;

    // === Widgets (bind in WBP_ColorPickerButton) ===
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    class UBorder* ColorBorder;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    class UBorder* SelectionOutline;  // NEW: white ring

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    class UImage* LockIcon;

private:
    bool bIsSelected = false;
};
