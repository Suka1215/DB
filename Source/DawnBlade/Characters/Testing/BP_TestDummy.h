#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "BP_TestDummy.generated.h"

UCLASS()
class DAWNBLADE_API ABP_TestDummy : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABP_TestDummy();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Ability System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	// Your Attribute Sets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UDBAttr_Primary* PrimaryAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UDBAttr_Combat* CombatAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UDBAttr_Resistance* ResistanceAttributes;

	// Visual feedback
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* HealthBarWidget;

	// Enemy tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FGameplayTag EnemyTag;

	// Visual feedback when hit
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FlashOnHit();

	// Get health percentage for UI
	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetHealthPercent() const;

private:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	FTimerHandle FlashTimer;

	// Bind to attribute changes
	void OnHealthChanged(const struct FOnAttributeChangeData& Data);
};