#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDBW_AbilityDebugPanel.generated.h"

class UVerticalBox;
class UButton;
class UTextBlock;
class ADBPlayerCharacter;

/** Small helper UObject that stores the AbilityClass and calls back into the panel on click */
UCLASS()
class DAWNBLADE_API UDBW_ButtonRelay : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() TWeakObjectPtr<class UDBW_AbilityDebugPanel> OwnerPanel;
	UPROPERTY() TSubclassOf<class UGameplayAbility> AbilityClass;

	UFUNCTION()
	void HandleClicked(); // no params; bound to Button->OnClicked
};

UCLASS()
class DAWNBLADE_API UDBW_AbilityDebugPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitForCharacter(ADBPlayerCharacter* InCharacter);

	/** Ability clicked entry point (called by relay) — must be public so relay can call it */
	UFUNCTION()
	void OnSpellClicked(UDBW_ButtonRelay* Relay);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY() TArray<TObjectPtr<UDBW_ButtonRelay>> ButtonRelays;  // keep relays alive
	
	// Optional designer VerticalBox named "SpellList".
	UPROPERTY(meta=(BindWidgetOptional)) UVerticalBox* SpellList;

	// Fallback container if designer didn’t provide one
	UPROPERTY() UVerticalBox* RootBox = nullptr;

	// Top buttons
	UFUNCTION() void OnSetWarrior();
	UFUNCTION() void OnSetLightMage();
	UFUNCTION() void RebuildSpellList();
	UFUNCTION() void OnLearnCureBook();

	UPROPERTY() TWeakObjectPtr<ADBPlayerCharacter> OwningChar;

	// building helpers
	void BuildLayoutIfNoDesigner();
	void AddHeaderRow(const FString& Label, UPanelWidget* Parent);
	void AddTopButtons(UPanelWidget* Parent);
	void AddSpellButton(TSubclassOf<class UGameplayAbility> AbilityClass);
};
