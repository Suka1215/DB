#include "GUI/HUD/DBW_SkillButton.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Algo/MaxElement.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UDBW_SkillButton::InitFromAbility(TSubclassOf<UGameplayAbility> InAbility,
                                       UAbilitySystemComponent* InASC,
                                       const FText& InKeyHint)
{
	AbilityClass = InAbility;
	ASC          = InASC;
	KeyHint      = InKeyHint;

	// Basic UI
	if (KeyText)
	{
		KeyText->SetText(KeyHint);
	}
	if (IconImage)
	{
		IconImage->SetOpacity(1.f); // hook up real icon later
	}

	// Derive a cooldown tag if you don't store one in the GA:
	// Convention: "Cooldown.<AbilityBPName>"
	if (!CooldownTag.IsValid() && AbilityClass)
	{
		const FString TagName = FString::Printf(TEXT("Cooldown.%s"), *AbilityClass->GetName());
		CooldownTag = FGameplayTag::RequestGameplayTag(FName(*TagName), /*ErrorIfNotFound*/ false);
	}

	// Subscribe to tag add/remove events
	if (ASC && CooldownTag.IsValid())
	{
		CooldownDelegateHandle =
			ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)
			   .AddUObject(this, &UDBW_SkillButton::OnCooldownTagChanged);
	}

	RefreshCooldown();
}

void UDBW_SkillButton::OnCooldownTagChanged(FGameplayTag /*Tag*/, int32 /*NewCount*/)
{
	// Delegate signature requires params; we ignore them and just refresh.
	RefreshCooldown();
}

void UDBW_SkillButton::RefreshCooldown()
{
	if (!ASC || !CooldownTag.IsValid())
	{
		UpdateVisualsForCooldown(0.f, 0.f);
		return;
	}

	// Build a query that matches active effects "owning" (granting) this cooldown tag.
	FGameplayTagContainer Tags;
	Tags.AddTag(CooldownTag);
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(Tags);

	// Time remaining for matching effects
	const TArray<float> RemainingTimes = ASC->GetActiveEffectsTimeRemaining(Query);

	float TimeRemaining = 0.f;
	if (RemainingTimes.Num() > 0)
	{
		const float* MaxPtr = Algo::MaxElement(RemainingTimes);
		TimeRemaining = MaxPtr ? *MaxPtr : 0.f;
	}

	// Durations for matching effects
	const TArray<float> Durations = ASC->GetActiveEffectsDuration(Query);
	float Duration = 0.f;
	if (Durations.Num() > 0)
	{
		const float* MaxPtr = Algo::MaxElement(Durations);
		Duration = MaxPtr ? *MaxPtr : 0.f;
	}

	UpdateVisualsForCooldown(TimeRemaining, Duration);
}

void UDBW_SkillButton::UpdateVisualsForCooldown(float TimeRemaining, float Duration)
{
	if (!CooldownOverlay)
	{
		return;
	}

	if (Duration <= 0.f || TimeRemaining <= 0.f)
	{
		// No cooldown
		CooldownOverlay->SetVisibility(ESlateVisibility::Hidden);
		CooldownOverlay->SetPercent(0.f);
	}
	else
	{
		CooldownOverlay->SetVisibility(ESlateVisibility::Visible);
		CooldownOverlay->SetPercent(TimeRemaining / Duration);
	}
}

void UDBW_SkillButton::NativeDestruct()
{
	if (ASC && CooldownTag.IsValid() && CooldownDelegateHandle.IsValid())
	{
		ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)
		   .Remove(CooldownDelegateHandle);
	}
	Super::NativeDestruct();
}

void UDBW_SkillButton::PlayPress()
{
	if (Anim_Press) PlayAnimation(Anim_Press, 0.f, 1);
}
