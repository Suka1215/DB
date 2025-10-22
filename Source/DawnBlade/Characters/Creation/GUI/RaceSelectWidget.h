#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "RaceSelectWidget.generated.h"

class ACharacterRaceLineup;
class UButton;
class UTextBlock;

UCLASS()
class DAWNBLADE_API URaceSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonPrev;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonNext;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonConfirm;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextRaceName;

	// Reference to lineup actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select")
	TWeakObjectPtr<ACharacterRaceLineup> LineupActor;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPrevClicked();

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();

	void UpdateUI();
};
