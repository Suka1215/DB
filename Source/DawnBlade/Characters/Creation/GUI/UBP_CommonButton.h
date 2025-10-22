// BP_CommonButton.h

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "UBP_CommonButton.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class DAWNBLADE_API UBP_CommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	// === Dynamic properties ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(ExposeOnSpawn="true"))
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(ExposeOnSpawn="true"))
	FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(ExposeOnSpawn="true"))
	float IconWidth = 64.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button", meta=(ExposeOnSpawn="true"))
	float IconHeight = 64.f;

protected:
	virtual void NativePreConstruct() override;

	// === Bound Widgets ===
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Button")
	UImage* IconImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Button")
	UTextBlock* LabelText;
};