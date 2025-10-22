#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "DBW_SkillButton.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UImage;
class UTextBlock;
class UProgressBar;

UCLASS()
class DAWNBLADE_API UDBW_SkillButton : public UUserWidget
{
	GENERATED_BODY()

public:
	/** ASC-based init keeps the widget independent of character classes */
	UFUNCTION(BlueprintCallable, Category="DB|UI")
	void InitFromAbility(TSubclassOf<UGameplayAbility> InAbility, UAbilitySystemComponent* InASC, const FText& InKeyHint);

	/** Manual refresh (e.g., after init or when pressed) */
	UFUNCTION(BlueprintCallable, Category="DB|UI")
	void RefreshCooldown();

protected:
	virtual void NativeDestruct() override;

	/** Delegate bridge that matches RegisterGameplayTagEvent signature */
	UFUNCTION()
	void OnCooldownTagChanged(FGameplayTag Tag, int32 NewCount);

	/** Update visuals (overlay fill/text/etc.) */
	void UpdateVisualsForCooldown(float TimeRemaining, float Duration);

protected:
	// ---------- Data ----------
	UPROPERTY()
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY()
	UAbilitySystemComponent* ASC = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DB|UI")
	FGameplayTag CooldownTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DB|UI")
	FText KeyHint;

	// ---------- Widgets (bind in WBP) ----------
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	UImage* IconImage = nullptr;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	UTextBlock* KeyText = nullptr;

	// ---------- Widget Anim -------------
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional)) UWidgetAnimation* Anim_Press;
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional)) UWidgetAnimation* Anim_CooldownStart;

	// Call this when user activates via keyboard/gamepad (optional)
	UFUNCTION(BlueprintCallable) void PlayPress();

	/** Radial/linear overlay that fills during cooldown (0..1) */
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	UProgressBar* CooldownOverlay = nullptr;

	// ---------- Handles ----------
	FDelegateHandle CooldownDelegateHandle;
};
