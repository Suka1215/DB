// DBBaseWeaponAnimInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/DBWeaponAnimationSet.h"
#include "DBBaseWeaponAnimInstance.generated.h"

UCLASS()
class DAWNBLADE_API UDBBaseWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// ========================================
	// WEAPON DATA (from Weapon Component)
	// Changed to public and BlueprintReadWrite for AnimGraph access
	// ========================================
    
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	class UDBWeaponComponent* WeaponComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	UDBWeaponAnimationSet* CurrentWeaponData;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bWeaponDrawn;

	// ========================================
	// MOVEMENT STATE
	// ========================================
    
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float Speed;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float Direction;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsFalling;

	// ========================================
	// REFERENCES
	// ========================================
    
	UPROPERTY(BlueprintReadWrite, Category = "References")
	class ACharacter* OwningCharacter;

	// ========================================
	// FOR ANIMGRAPH
	// ========================================
    
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon Animation")
	UBlendSpace* GetLocomotionBlendspace() const;

protected:
	// Keep protected/private helper functions here if needed
};