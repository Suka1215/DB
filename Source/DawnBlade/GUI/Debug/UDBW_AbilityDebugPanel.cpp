#include "GUI/Debug/UDBW_AbilityDebugPanel.h"

#include "Abilities/GameplayAbility.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

#include "Components/DBJobComponent.h"
#include "Components/DBLoadoutComponent.h"
#include "Types/EDBJob.h"
#include "Abilities/Spells/DBGA_Cure.h"
#include "Characters/DBPlayerCharacter.h"

// ===== Relay =====
void UDBW_ButtonRelay::HandleClicked()
{
	if (OwnerPanel.IsValid())
	{
		OwnerPanel->OnSpellClicked(this);
	}
}

// ===== Panel =====
void UDBW_AbilityDebugPanel::InitForCharacter(ADBPlayerCharacter* InCharacter)
{
	OwningChar = InCharacter;
	if (!InCharacter) return;

	if (UDBJobComponent* Job = InCharacter->FindComponentByClass<UDBJobComponent>())
	{
		Job->OnJobChanged.AddDynamic(this, &UDBW_AbilityDebugPanel::RebuildSpellList);
		Job->OnJobLevelChanged.AddDynamic(this, &UDBW_AbilityDebugPanel::RebuildSpellList);
		Job->OnActiveGrantsChanged.AddDynamic(this, &UDBW_AbilityDebugPanel::RebuildSpellList);
		Job->ClientRequestGrantsRefresh(); 
	}
}

void UDBW_AbilityDebugPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// Build fallback layout if designer didn't provide SpellList
	if (!SpellList)
	{
		BuildLayoutIfNoDesigner();
	}
}

void UDBW_AbilityDebugPanel::BuildLayoutIfNoDesigner()
{
	// Make a root vertical box if none exists
	if (!WidgetTree->RootWidget)
	{
		RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
		WidgetTree->RootWidget = RootBox;
	}
	else
	{
		// If there is already a root, try to use it if it’s a panel
		RootBox = Cast<UVerticalBox>(WidgetTree->RootWidget);
		if (!RootBox)
		{
			// Replace root with a vertical box
			RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
			WidgetTree->RootWidget = RootBox;
		}
	}

	AddHeaderRow(TEXT("DawnBlade – Ability Debug"), RootBox);
	AddTopButtons(RootBox);

	// “Known Abilities” header + list
	AddHeaderRow(TEXT("Known Abilities"), RootBox);
	SpellList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SpellList"));
	RootBox->AddChild(SpellList);
}

void UDBW_AbilityDebugPanel::AddHeaderRow(const FString& Label, UPanelWidget* Parent)
{
	if (!Parent) return;

	UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Text->SetText(FText::FromString(Label));
	Text->SetJustification(ETextJustify::Center);
	Text->SetAutoWrapText(true);

	Parent->AddChild(Text);
}

void UDBW_AbilityDebugPanel::AddTopButtons(UPanelWidget* Parent)
{
	UE_LOG(LogTemp, Warning, TEXT("AddTopButtons called"));

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	Parent->AddChild(Row);

	// Warrior
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Txt->SetText(FText::FromString(TEXT("Set Warrior")));
		Btn->AddChild(Txt);

		UE_LOG(LogTemp, Warning, TEXT("Binding OnSetWarrior"));
		Btn->OnClicked.AddDynamic(this, &UDBW_AbilityDebugPanel::OnSetWarrior);

		Row->AddChildToHorizontalBox(Btn);
	}

	// Light Mage
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Txt->SetText(FText::FromString(TEXT("Set Light Mage")));
		Btn->AddChild(Txt);

		UE_LOG(LogTemp, Warning, TEXT("Binding OnSetLightMage"));
		Btn->OnClicked.AddDynamic(this, &UDBW_AbilityDebugPanel::OnSetLightMage);

		Row->AddChildToHorizontalBox(Btn);
	}
	// Learn Cure Book
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Txt->SetText(FText::FromString(TEXT("Learn: Cure Book")));
		Btn->AddChild(Txt);
		Btn->OnClicked.AddDynamic(this, &UDBW_AbilityDebugPanel::OnLearnCureBook);
		Row->AddChildToHorizontalBox(Btn);
	}
}

void UDBW_AbilityDebugPanel::OnLearnCureBook()
{
	if (!OwningChar.IsValid()) return;
	if (UDBJobComponent* Job = OwningChar->FindComponentByClass<UDBJobComponent>())
	{
		Job->ServerLearnAbility(UDBGA_Cure::StaticClass());   // <— HERE
	}
}

void UDBW_AbilityDebugPanel::OnSetWarrior()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSetWarrior clicked!"));

	if (OwningChar.IsValid())
	{
		if (UDBJobComponent* Job = OwningChar->FindComponentByClass<UDBJobComponent>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling ServerSetCurrentJob: Warrior"));
			Job->ServerSetCurrentJob(EDBJob::Warrior);
		}
	}

	RebuildSpellList();
}

void UDBW_AbilityDebugPanel::OnSetLightMage()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSetLightMage clicked!"));

	if (OwningChar.IsValid())
	{
		if (UDBJobComponent* Job = OwningChar->FindComponentByClass<UDBJobComponent>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling ServerSetCurrentJob: LightMage"));
			Job->ServerSetCurrentJob(EDBJob::LightMage);
		}
	}

	RebuildSpellList();
}

void UDBW_AbilityDebugPanel::RebuildSpellList()
{
	if (!SpellList) return;
	SpellList->ClearChildren();

	if (!OwningChar.IsValid()) return;

	TArray<TSubclassOf<UGameplayAbility>> Known;
	OwningChar->GetKnownAbilities(Known);

	for (auto& AbilityClass : Known)
	{
		AddSpellButton(AbilityClass);
	}
}

void UDBW_AbilityDebugPanel::AddSpellButton(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass || !SpellList) return;

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	SpellList->AddChild(Row);

	UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Txt->SetText(FText::FromString(AbilityClass->GetName()));
	Btn->AddChild(Txt);

	UDBW_ButtonRelay* Relay = NewObject<UDBW_ButtonRelay>(this);
	Relay->OwnerPanel   = this;
	Relay->AbilityClass = AbilityClass;

	// Keep it alive
	ButtonRelays.Add(Relay);

	Btn->OnClicked.AddDynamic(Relay, &UDBW_ButtonRelay::HandleClicked);
	Row->AddChildToHorizontalBox(Btn);
}

void UDBW_AbilityDebugPanel::OnSpellClicked(UDBW_ButtonRelay* Relay)
{
	if (!Relay || !OwningChar.IsValid()) return;
	OwningChar->ActivateAbilityByClass(Relay->AbilityClass);
}
