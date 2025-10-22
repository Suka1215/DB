#include "Player/DBHUD.h"
#include "Blueprint/UserWidget.h"

void ADBHUD::BeginPlay()
{
	Super::BeginPlay();
	if (CombatPaletteClass)
	{
		CombatPaletteWidget = CreateWidget(GetWorld(), CombatPaletteClass);
		if (CombatPaletteWidget)
		{
			CombatPaletteWidget->AddToViewport(10);
			CombatPaletteWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void ADBHUD::ShowCombatPalette(bool bShow)
{
	if (CombatPaletteWidget) CombatPaletteWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
