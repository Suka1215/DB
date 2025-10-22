#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FeaturePresetButton.generated.h"

// Delegate when a preset button is clicked
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFeaturePresetSelectedSignature, int32, PresetIndex);

UCLASS()
class DAWNBLADE_API UFeaturePresetButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFeaturePresetButton(const FObjectInitializer& ObjectInitializer);

	// Public API
	UFUNCTION(BlueprintCallable, Category = "Character Creator")
	void SetPresetData(const FText& InDisplayName, UTexture2D* InThumbnail, int32 InUnlockLevel, bool bInRequiresPremium, int32 InPresetIndex);

	UFUNCTION(BlueprintCallable, Category = "Character Creator")
	void SetPresetSelected(bool bIsSelected);

	UFUNCTION(BlueprintPure, Category = "Character Creator")
	UTexture2D* GetPreviewThumbnail() const { return PreviewThumbnail; }

	/** Apply enforced cell size (called from parent widget’s UpdateThumbnailSizes) */
	UFUNCTION(BlueprintCallable, Category = "Character Creator")
	void ApplyThumbnailSize(const FVector2D& NewSize);

	// Delegate fired when clicked
	UPROPERTY(BlueprintAssignable, Category = "Character Creator")
	FOnFeaturePresetSelectedSignature OnFeaturePresetSelected;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	void UpdateUI();

	UFUNCTION()
	void HandleOnClicked();

public:
	// Stored data accessible by other systems
	UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
	int32 PresetIndex = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
	bool bRequiresPremium = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Creator")
	int32 UnlockLevel = 1;

	// Widget bindings
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	class USizeBox* SizeBox_Root;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	class UImage* ThumbnailImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	class UCommonTextBlock* Label_DisplayName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	class UImage* LockIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	class UBorder* SelectionBorder;

private:
	FText DisplayName;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> PreviewThumbnail;

	bool bIsSelected = false;
};
