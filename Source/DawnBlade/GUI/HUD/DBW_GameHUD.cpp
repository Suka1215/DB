// UDBW_GameHUD.cpp
#include "GUI/HUD/DBW_GameHUD.h"
#include "GUI/HUD/DBW_SkillButton.h"

#include "Characters/DBPlayerCharacter.h"
#include "Components/DBJobComponent.h"
#include "Components/DBLoadoutComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/DBPlayerCharacter.h"

#include "GUI/HUD/DBW_PartyList.h"
#include "Engine/Texture2D.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/PanelWidget.h"

#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/AdvancedWidgets/Public/Components/RadialSlider.h"

void UDBW_GameHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDBW_GameHUD::NativeDestruct()
{
	UnbindAttributes();

	if (Loadout.IsValid())
	{
		Loadout->OnActiveSetChanged.RemoveDynamic(this, &UDBW_GameHUD::HandleActiveSetChanged);
	}
	if (Job.IsValid())
	{
		Job->OnJobChanged.RemoveDynamic(this, &UDBW_GameHUD::HandleJobChanged);
		Job->OnJobLevelChanged.RemoveDynamic(this, &UDBW_GameHUD::HandleJobLevelChanged);
	}

	Super::NativeDestruct();
}

void UDBW_GameHUD::InitForCharacter(ADBPlayerCharacter* InCharacter)
{
	OwnerChar = InCharacter;
	if (!InCharacter) return;

	ASC     = InCharacter->GetAbilitySystemComponent();
	Job     = InCharacter->FindComponentByClass<UDBJobComponent>();
	Loadout = InCharacter->FindComponentByClass<UDBLoadoutComponent>();

	BindAttributes();
	RebuildTopLeft();
	RebuildHotbar();

	if (Loadout.IsValid())
	{
		// OnActiveSetChanged expects (int32 NewIndex)
		Loadout->OnActiveSetChanged.AddDynamic(this, &UDBW_GameHUD::HandleActiveSetChanged);
	}
    
	if (Job.IsValid())
	{
		Job->OnJobChanged.AddDynamic(this, &UDBW_GameHUD::HandleJobChanged);
		Job->OnJobLevelChanged.AddDynamic(this, &UDBW_GameHUD::HandleJobLevelChanged);
	}

	if (PartyListWidget) // BindWidget PartyListWidget in GameHUD BP
	{
		TArray<FPartyMemberData> AllPlayers;

		// Example 6 party members
		FPartyMemberData Kael; // YOU
		Kael.Name = TEXT("Kael");
		Kael.Level = 36;
		Kael.Job = EDBJob::Warrior;
		Kael.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/warrior_icon.warrior_icon"));
		Kael.CurrentHP = 1200.f;
		Kael.MaxHP = 3000.f;
		Kael.CurrentMP = 1100.f;
		Kael.MaxMP = 1500.f;
		Kael.bIsLocalPlayer = true;
		AllPlayers.Add(Kael);

		FPartyMemberData Luna;
		Luna.Name = TEXT("Luna");
		Luna.Level = 34;
		Luna.Job = EDBJob::LightMage;
		Luna.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/warrior_icon.warrior_icon"));
		Luna.CurrentHP = 900.f;
		Luna.MaxHP = 2400.f;
		Luna.CurrentMP = 2000.f;
		Luna.MaxMP = 2200.f;
		AllPlayers.Add(Luna);

		FPartyMemberData Riven;
		Riven.Name = TEXT("Riven");
		Riven.Level = 35;
		Riven.Job = EDBJob::Paladin;
		Riven.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/sentinel.sentinel"));
		Riven.CurrentHP = 1500.f;
		Riven.MaxHP = 2800.f;
		Riven.CurrentMP = 300.f;
		Riven.MaxMP = 600.f;
		AllPlayers.Add(Riven);

		FPartyMemberData Sera;
		Sera.Name = TEXT("Sera");
		Sera.Level = 33;
		Sera.Job = EDBJob::Warrior;
		Sera.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/warrior_icon.warrior_icon"));
		Sera.CurrentHP = 800.f;
		Sera.MaxHP = 2000.f;
		Sera.CurrentMP = 1800.f;
		Sera.MaxMP = 2200.f;
		AllPlayers.Add(Sera);

		FPartyMemberData Thorne;
		Thorne.Name = TEXT("Thorne");
		Thorne.Level = 32;
		Thorne.Job = EDBJob::Paladin;
		Thorne.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/warrior_icon.warrior_icon"));
		Thorne.CurrentHP = 1000.f;
		Thorne.MaxHP = 2200.f;
		Thorne.CurrentMP = 900.f;
		Thorne.MaxMP = 1200.f;
		AllPlayers.Add(Thorne);

		FPartyMemberData Mira;
		Mira.Name = TEXT("Mira");
		Mira.Level = 31;
		Mira.Job = EDBJob::LightMage;
		Mira.JobIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Game/GUI/Images/Jobs/warrior_icon.warrior_icon"));
		Mira.CurrentHP = 1700.f;
		Mira.MaxHP = 3200.f;
		Mira.CurrentMP = 700.f;
		Mira.MaxMP = 1000.f;
		AllPlayers.Add(Mira);
		
		PartyListWidget->InitParty(AllPlayers);
	}
}

void UDBW_GameHUD::BindAttributes()
{
	UnbindAttributes();
	if (!ASC.IsValid()) return;

	// Health
	HealthDH = ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetHealthAttribute())
		.AddUObject(this, &UDBW_GameHUD::OnHealthChanged);
	
	MaxHealthDH = ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetMaxHealthAttribute())
		.AddUObject(this, &UDBW_GameHUD::OnHealthChanged);
	
	// Mana
	ManaDH = ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetManaAttribute())
		.AddUObject(this, &UDBW_GameHUD::OnManaChanged);

	MaxManaDH = ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetMaxManaAttribute())
		.AddUObject(this, &UDBW_GameHUD::OnManaChanged);

	TPDH = ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Combat::GetTPAttribute())
		.AddUObject(this, &UDBW_GameHUD::OnTpChanged);

	// initial paint
	if (ASC.IsValid())
	{
		const float HP   = ASC->GetNumericAttribute(UDBAttr_Primary::GetHealthAttribute());
		const float HPMax= ASC->GetNumericAttribute(UDBAttr_Primary::GetMaxHealthAttribute());
		const float MP   = ASC->GetNumericAttribute(UDBAttr_Primary::GetManaAttribute());
		const float MPMax= ASC->GetNumericAttribute(UDBAttr_Primary::GetMaxManaAttribute());
		const float TP = ASC->GetNumericAttribute(UDBAttr_Combat::GetTPAttribute());
		const float TPMax = ASC->GetNumericAttribute(UDBAttr_Combat::GetMaxTPAttribute());

		DisplayedHP = HP;
		TargetHP    = HP;
		HPMaxCached = HPMax;

		DisplayedMP = MP;
		TargetMP    = MP;
		MPMaxCached = MPMax;

		DisplayedTP = TP;
		TargetTP = TP;
		TPMaxCached = TPMax;

		UpdateHPUI();  // helper we’ll add
		UpdateMPUI();
		
		if (HPBar) HPBar->SetPercent(HPMax>0 ? HP/HPMax : 0.f);
		if (MPBar) MPBar->SetPercent(MPMax>0 ? MP/MPMax : 0.f);

		if (CurrentHPText && CurrentHPMax)
		{
			CurrentHPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				FMath::FloorToInt(HP))));

			CurrentHPMax->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				FMath::FloorToInt(HPMax))));
		}

		if (CurrentMPText && CurrentMPMax)
		{
			CurrentMPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				FMath::FloorToInt(MP))));

			CurrentMPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				 FMath::FloorToInt(MPMax))));
		}

		if (CurrentTPText)
		{
			CurrentTPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				FMath::FloorToInt(TP))));
		}
	}
}

void UDBW_GameHUD::UnbindAttributes()
{
	if (!ASC.IsValid()) return;
	if (HealthDH.IsValid()) { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetHealthAttribute()).Remove(HealthDH); HealthDH.Reset(); }
	if (MaxHealthDH.IsValid()) { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetMaxHealthAttribute()).Remove(MaxHealthDH); MaxHealthDH.Reset(); }
	if (MaxManaDH.IsValid()) { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetMaxManaAttribute()).Remove(MaxManaDH); MaxManaDH.Reset(); }
	if (ManaDH.IsValid())   { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Primary::GetManaAttribute()).Remove(ManaDH); ManaDH.Reset(); }
	if (TPDH.IsValid()) { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Combat::GetTPAttribute()).Remove(TPDH); TPDH.Reset(); }
	if (MaxTPDH.IsValid()) { ASC->GetGameplayAttributeValueChangeDelegate(UDBAttr_Combat::GetMaxTPAttribute()).Remove(MaxTPDH); MaxTPDH.Reset(); }
}

void UDBW_GameHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (HPBar)
	{
		const float Max     = ASC.IsValid() ? ASC->GetNumericAttribute(UDBAttr_Primary::GetMaxHealthAttribute()) : 0.f;
		const float Current = Data.NewValue;
		const float Percent = Max > 0 ? Current / Max : 0.f;

		// Update normal progress bar
		HPBar->SetPercent(Percent);

		// Update text (e.g., "540 / 1000")
		if (CurrentHPText)
		{
			CurrentHPText->SetText(FText::FromString(
				FString::Printf(TEXT("%d / %d"),
					FMath::RoundToInt(Current),
					FMath::RoundToInt(Max))));
		}

		// Cache for smoothing/lerping if needed
		TargetHP    = Current;
		HPMaxCached = Max;
	}

	// Play a flash animation if bound
	if (Anim_HitFlash)
	{
		PlayAnimation(Anim_HitFlash, 0.f, 1);
	}
}

void UDBW_GameHUD::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if (MPBar)
	{
		const float Max     = ASC.IsValid() ? ASC->GetNumericAttribute(UDBAttr_Primary::GetMaxManaAttribute()) : 0.f;
		const float Current = Data.NewValue;
		const float Percent = Max > 0 ? Current / Max : 0.f;

		// Update normal progress bar
		MPBar->SetPercent(Percent);

		// Update text (e.g., "120 / 250")
		if (CurrentMPText)
		{
			CurrentMPText->SetText(FText::FromString(
				FString::Printf(TEXT("%d / %d"),
					FMath::RoundToInt(Current),
					FMath::RoundToInt(Max))));
		}

		// Cache for smoothing/lerping if needed
		TargetMP    = Current;
		MPMaxCached = Max;
	}

	// Play a mana pulse animation if bound
	if (Anim_ManaPulse)
	{
		PlayAnimation(Anim_ManaPulse, 0.f, 1);
	}
}

void UDBW_GameHUD::OnTpChanged(const FOnAttributeChangeData& Data)
{
	if (TPBar)
	{
		const float Max     = ASC.IsValid() ? ASC->GetNumericAttribute(UDBAttr_Combat::GetMaxTPAttribute()) : 0.f;
		const float Current = Data.NewValue;
		const float Percent = Max > 0 ? Current / Max : 0.f;

		TPBar->SetValue(Percent);

		if (CurrentTPText)
		{
			CurrentTPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				FMath::RoundToInt(Current))));
		}

		TargetTP    = Current;
		TPMaxCached = Max;

		// Play a mana pulse animation if bound
		if (Anim_ManaPulse)
		{
			PlayAnimation(Anim_ManaPulse, 0.f, 1);
		}
	}
}

void UDBW_GameHUD::OnLevelChanged()
{
}

void UDBW_GameHUD::RebuildTopLeft()
{
	if (OwnerChar.IsValid())
	{
		if (PlayerName) PlayerName->SetText(FText::FromString(OwnerChar->GetName()));
	}
	// LevelText: if you store level in Job
	if (Job.IsValid() && LevelText)
	{
		LevelText->SetText(FText::AsNumber(Job->GetLevel()));
	}
}

static FText KeyText(int32 Index)
{
	// Map 0..5 to strings (placeholder; you can pass from your input setup)
	static const TCHAR* Keys[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("Q"), TEXT("E") };
	return FText::FromString(Keys[FMath::Clamp(Index,0,5)]);
}

void UDBW_GameHUD::RebuildHotbar()
{
	if (!OwnerChar.IsValid() || !Loadout.IsValid()) return;

	const auto* Set = Loadout->GetActiveSet();
	if (!Set) return;

	// Clear existing
	if (HotbarPrimary)   HotbarPrimary->ClearChildren();
	if (HotbarSecondary) HotbarSecondary->ClearChildren();

	// Fill two rows of three (like your ref pic), and optional Ultimate/Dodge
	for (int32 i=0;i<Set->Entries.Num();++i)
	{
		const auto& E = Set->Entries[i];
		if (!E.Ability) continue;

		UDBW_SkillButton* Btn = MakeSkillButton(E.Ability, KeyText(i));
		if (!Btn) continue;

		if (i < 3 && HotbarPrimary)   HotbarPrimary->AddChild(Btn);
		else if (i < 6 && HotbarSecondary) HotbarSecondary->AddChild(Btn);
		else
		{
			// Slot extras: 6->Ultimate, 7->Dodge as an example
			if (i==6 && UltimateSlot) UltimateSlot->InitFromAbility(E.Ability, OwnerChar->GetAbilitySystemComponent(), FText::FromString(TEXT("R")));
			if (i==7 && DodgeSlot)    DodgeSlot->InitFromAbility(E.Ability,    OwnerChar->GetAbilitySystemComponent(), FText::FromString(TEXT("Space")));
		}
	}
}

// void UDBW_GameHUD::RebuildPartyList(const TArray<FPartyMemberData>& InPartyData)
// {
// 	CachedPartyData = InPartyData;
// 	
// 	if (!PartyListWidget || !PartyMemberWidgetClass)
// 	{
// 		PartyListWidget->ClearChildren();
//
// 		for (const FPartyMemberData& Data : CachedPartyData)
// 		{
// 			UDBW_PartyMember* Entry = CreateWidget<UDBW_PartyMember>(GetOwningPlayer(), PartyMemberWidgetClass);
// 			if (!Entry) return;
//
// 			Entry->InitPartyMember(Data);
// 			PartyListBox->AddChild(Entry);
// 		}
// 	}	
// }

UDBW_SkillButton* UDBW_GameHUD::MakeSkillButton(TSubclassOf<UGameplayAbility> AbilityClass, const FText& KeyHint)
{
	if (!AbilityClass || !OwnerChar.IsValid()) return nullptr;

	// Use the BP based on UDBW_SkillButton (or create native)
	UClass* WidgetClass = UDBW_SkillButton::StaticClass();
	UDBW_SkillButton* Btn = CreateWidget<UDBW_SkillButton>(GetOwningPlayer(), WidgetClass);
	if (!Btn) return nullptr;

	Btn->InitFromAbility(AbilityClass, OwnerChar->GetAbilitySystemComponent(), KeyHint);
	return Btn;
}

void UDBW_GameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Smoothly move displayed values toward target
	DisplayedHP = FMath::FInterpTo(DisplayedHP, TargetHP, InDeltaTime, BarInterpSpeed);
	DisplayedMP = FMath::FInterpTo(DisplayedMP, TargetMP, InDeltaTime, BarInterpSpeed);

	UpdateHPUI();
	UpdateMPUI();
}

void UDBW_GameHUD::UpdateHPUI()
{
	if (HPBar && HPMaxCached > 0.f)
	{
		HPBar->SetPercent(DisplayedHP / HPMaxCached);
	}
	if (CurrentHPText && CurrentHPMax)
	{
		CurrentHPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
			FMath::RoundToInt(DisplayedHP))));

		CurrentHPMax->SetText(FText::FromString(FString::Printf(TEXT("%d"),
			FMath::RoundToInt(HPMaxCached))));
	}
}

void UDBW_GameHUD::UpdateMPUI()
{
	if (MPBar && MPMaxCached > 0.f)
	{
		MPBar->SetPercent(DisplayedMP / MPMaxCached);
	}
	if (CurrentMPText && CurrentMPMax)
	{
		CurrentMPText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
			FMath::RoundToInt(DisplayedMP))));

		CurrentMPMax->SetText(FText::FromString(FString::Printf(TEXT("%d"),
			FMath::RoundToInt(MPMaxCached))));
	}
}


void UDBW_GameHUD::ClearPanelChildren(UWidget* Panel)
{
	if (auto* Box = Cast<UHorizontalBox>(Panel)) Box->ClearChildren();
	if (auto* VBox= Cast<UVerticalBox>(Panel))   VBox->ClearChildren();
	if (auto* Grid= Cast<UUniformGridPanel>(Panel)) Grid->ClearChildren();
}

void UDBW_GameHUD::HandleActiveSetChanged(int32)
{
	RebuildHotbar();
}

void UDBW_GameHUD::HandleJobChanged()
{
	RebuildTopLeft();
	RebuildHotbar();
}

void UDBW_GameHUD::HandleJobLevelChanged()
{
	RebuildTopLeft();
	RebuildHotbar();
}
