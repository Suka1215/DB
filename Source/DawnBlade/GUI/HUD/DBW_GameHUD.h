// UDBW_GameHUD.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/RadialSlider.h"
#include "AbilitySystemComponent.h"
#include "DBW_PartyMember.h"
#include "DBW_GameHUD.generated.h"

// Forward declarations
class UTextBlock;
class UImage;
class UHorizontalBox;
class UVerticalBox;
class UUniformGridPanel;
class UProgressBar;
class UDBW_SkillButton;
class UAbilitySystemComponent;
class ADBPlayerCharacter;
class UDBJobComponent;
class UDBLoadoutComponent;
class UDBW_PartyList;

UCLASS()
class DAWNBLADE_API UDBW_GameHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========================================================================
	// PUBLIC INTERFACE
	// ========================================================================
	
	UFUNCTION(BlueprintCallable, Category="DB|HUD")
	void InitForCharacter(ADBPlayerCharacter* InCharacter);

protected:
	// ========================================================================
	// WIDGET LIFECYCLE
	// ========================================================================
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI UPDATE METHODS
	// ========================================================================
	
	void UpdateHPUI();
	void UpdateMPUI();

	// ========================================================================
	// WIDGET BINDINGS - TOP LEFT PLAYER INFO
	// ========================================================================
	
	UPROPERTY(meta=(BindWidgetOptional)) UImage*       Portrait = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   PlayerName = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   LevelText = nullptr;
	
	// Health bar
	UPROPERTY(meta=(BindWidgetOptional)) UProgressBar* HPBar = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   CurrentHPText = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   CurrentHPMax = nullptr;
	
	// Mana bar
	UPROPERTY(meta=(BindWidgetOptional)) UProgressBar* MPBar = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   CurrentMPText = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*   CurrentMPMax = nullptr;
	
	// TP (Technique Points) bar
	UPROPERTY(meta=(BindWidgetOptional)) URadialSlider* TPBar = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*    CurrentTPText = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UTextBlock*    CurrentTPMax = nullptr;

	// Party List
	// UPROPERTY(meta=(BindWidgetOptional)) UVerticalBox* PartyListBox = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UDBW_PartyList* PartyListWidget = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="DB|HUD") TSubclassOf<UDBW_PartyMember> PartyMemberWidgetClass;

	void RebuildPartyList(const TArray<FPartyMemberData>& InPartyData);
	
	// Buffs/debuffs
	UPROPERTY(meta=(BindWidgetOptional)) UHorizontalBox* BuffsBox = nullptr;

	// ========================================================================
	// WIDGET BINDINGS - LEFT SIDE QUEST TRACKER
	// ========================================================================
	
	UPROPERTY(meta=(BindWidgetOptional)) UVerticalBox* QuestList = nullptr;

	// ========================================================================
	// WIDGET BINDINGS - TOP RIGHT UI
	// ========================================================================
	
	UPROPERTY(meta=(BindWidgetOptional)) UImage* Minimap = nullptr;
	UPROPERTY(meta=(BindWidgetOptional)) UUniformGridPanel* MenuGrid = nullptr;

	// ========================================================================
	// WIDGET BINDINGS - BOTTOM RIGHT ACTION BAR
	// ========================================================================
	
	UPROPERTY(meta=(BindWidgetOptional)) UHorizontalBox*   HotbarPrimary = nullptr;   // 3 small skills row
	UPROPERTY(meta=(BindWidgetOptional)) UHorizontalBox*   HotbarSecondary = nullptr; // 3 small skills row
	UPROPERTY(meta=(BindWidgetOptional)) UDBW_SkillButton* UltimateSlot = nullptr;    // big center button
	UPROPERTY(meta=(BindWidgetOptional)) UDBW_SkillButton* DodgeSlot = nullptr;       // small circular (jump/dash)
	UPROPERTY(meta=(BindWidgetOptional)) UDBW_SkillButton* InteractSlot = nullptr;    // optional

	// ========================================================================
	// WIDGET ANIMATIONS
	// ========================================================================
	
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional)) UWidgetAnimation* Anim_HitFlash;
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional)) UWidgetAnimation* Anim_ManaPulse;

	// ========================================================================
	// CONFIGURATION
	// ========================================================================
	
	UPROPERTY(EditAnywhere, Category="DB|HUD")
	float BarInterpSpeed = 5.0f;

	// ========================================================================
	// EVENT HANDLERS
	// ========================================================================
	
	UFUNCTION() void HandleActiveSetChanged(int32 NewIndex); // matches: OnActiveSetChanged(int32)
	UFUNCTION() void HandleJobChanged();                      // if your job delegate has no params
	UFUNCTION() void HandleJobLevelChanged();                 // if level-changed has int32

	// ========================================================================
	// UI STATE TRACKING
	// ========================================================================
	
	// Interpolated display values
	float DisplayedHP = 0.f;
	float TargetHP    = 0.f;
	float DisplayedMP = 0.f;
	float TargetMP    = 0.f;
	float DisplayedTP = 0.f;
	float TargetTP    = 0.f;

	// Cached maximum values
	float HPMaxCached = 0.f;
	float MPMaxCached = 0.f;
	float TPMaxCached = 0.f;

private:
	// ========================================================================
	// COMPONENT REFERENCES
	// ========================================================================
	
	UPROPERTY() TWeakObjectPtr<ADBPlayerCharacter>     OwnerChar;
	UPROPERTY() TWeakObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY() TWeakObjectPtr<UDBJobComponent>         Job;
	UPROPERTY() TWeakObjectPtr<UDBLoadoutComponent>     Loadout;

	// ========================================================================
	// DELEGATE HANDLES
	// ========================================================================
	
	// Attribute change delegates
	FDelegateHandle HealthDH, ManaDH, LevelDH, MaxHealthDH, MaxManaDH, TPDH, MaxTPDH;
	
	// Component change delegates
	FDelegateHandle SetChangedDH, JobChangedDH, JobLevelDH;

	// ========================================================================
	// Cached Party Data
	// ========================================================================
	TArray<FPartyMemberData> CachedPartyData;

	// ========================================================================
	// INTERNAL METHODS
	// ========================================================================
	
	// Attribute binding
	void BindAttributes();
	void UnbindAttributes();
	
	// UI rebuilding
	void RebuildHotbar();   // read Loadout->GetActiveSet()
	void RebuildTopLeft();
	
	// Attribute change callbacks
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnTpChanged(const FOnAttributeChangeData& Data);
	void OnLevelChanged(); // if you keep Level elsewhere, call this from a job event
	
	// Utility methods
	UDBW_SkillButton* MakeSkillButton(TSubclassOf<class UGameplayAbility> AbilityClass, const FText& KeyHint);
	void ClearPanelChildren(UWidget* Panel);
};