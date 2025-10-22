#include "RaceSelectWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CharacterRaceLineup.h"
#include "Kismet/GameplayStatics.h"

void URaceSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtonPrev) ButtonPrev->OnClicked.AddDynamic(this, &URaceSelectWidget::OnPrevClicked);
	if (ButtonNext) ButtonNext->OnClicked.AddDynamic(this, &URaceSelectWidget::OnNextClicked);
	if (ButtonConfirm) ButtonConfirm->OnClicked.AddDynamic(this, &URaceSelectWidget::OnConfirmClicked);

	UpdateUI();
}

void URaceSelectWidget::OnPrevClicked()
{
	if (LineupActor.IsValid())
	{
		LineupActor->PrevRace();
		UpdateUI();
	}
}

void URaceSelectWidget::OnNextClicked()
{
	if (LineupActor.IsValid())
	{
		LineupActor->NextRace();
		UpdateUI();
	}
}

void URaceSelectWidget::OnConfirmClicked()
{
	if (!LineupActor.IsValid()) return;

	FRacePreset Chosen = LineupActor->GetCurrentRace();

	// Save to GameInstance or pass to CharacterCreator
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		// TODO: Cast to your custom GameInstance and store race choice
	}

	// Travel to character creation level
	UGameplayStatics::OpenLevel(this, FName("CharacterCreator"));
}

void URaceSelectWidget::UpdateUI()
{
	if (LineupActor.IsValid() && TextRaceName)
	{
		FRacePreset Current = LineupActor->GetCurrentRace();
		TextRaceName->SetText(Current.DisplayName);
	}
}
