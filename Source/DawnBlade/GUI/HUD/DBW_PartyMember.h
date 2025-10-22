#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/PartyMemberData.h"
#include "DBW_PartyMember.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class UHorizontalBox;

UCLASS()
class DAWNBLADE_API UDBW_PartyMember : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitPartyMember(const FPartyMemberData& Data);

	UFUNCTION(BlueprintCallable)
	void UpdateFromData(const FPartyMemberData& Data);

protected:
	UPROPERTY(meta=(BindWidget)) UTextBlock* LevelText;
	UPROPERTY(meta=(BindWidget)) UTextBlock* NameText;
	UPROPERTY(meta=(BindWidget)) UProgressBar* HPBar;
	UPROPERTY(meta=(BindWidget)) UProgressBar* MPBar;
	UPROPERTY(meta=(BindWidget)) UImage* ClassIcon;
	UPROPERTY(meta=(BindWidget)) UHorizontalBox* Buffs_HBox;
};
