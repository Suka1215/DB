// DBWeaponComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/DBWeaponAnimationSet.h"
#include "DBWeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    Unarmed      UMETA(DisplayName = "Unarmed"),
    Sheathed     UMETA(DisplayName = "Sheathed"),
    Drawn        UMETA(DisplayName = "Drawn"),
    Engaging     UMETA(DisplayName = "Engaging"),
    Disengaging  UMETA(DisplayName = "Disengaging")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponStateChanged, EWeaponState, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDBWeaponComponent();

    // ========================================
    // WEAPON MANAGEMENT
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool CanEquipWeapon(UDBWeaponAnimationSet* WeaponData, FString& OutFailureReason) const;
    
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipWeapon(UDBWeaponAnimationSet* WeaponData);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void UnequipWeapon();

    // ========================================
    // WEAPON STATE
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ToggleWeapon();
    
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void DrawWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SheathWeapon();

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsWeaponDrawn() const { return WeaponState == EWeaponState::Drawn; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    EWeaponState GetWeaponState() const { return WeaponState; }

    // ========================================
    // ANIM NOTIFY CALLBACKS (Called by animation notifies)
    // ========================================
    
    UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
    void MoveWeaponsToDrawnSockets();
    
    UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
    void MoveWeaponsToSheathedSockets();

    UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
    void OnDrawComplete();
    
    UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
    void OnSheathComplete();

    // ========================================
    // COMBAT QUERIES
    // ========================================
    
    UFUNCTION(BlueprintPure, Category = "Weapon")
    UDBWeaponAnimationSet* GetCurrentWeaponData() const { return CurrentWeaponData; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    float GetAutoAttackRange() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    float GetEngageRange() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool CanAutoAttack() const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    UAnimMontage* GetRandomBasicAttack() const;

    // ========================================
    // EVENTS
    // ========================================
    
    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponStateChanged OnWeaponStateChanged;

protected:
    virtual void BeginPlay() override;

    // ========================================
    // CONFIGURATION
    // ========================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UDBWeaponAnimationSet* DefaultUnarmedData;

    // ========================================
    // RUNTIME STATE
    // ========================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    UDBWeaponAnimationSet* CurrentWeaponData;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    EWeaponState WeaponState;

    void PlayDrawMontage();
    void PlaySheathMontage();
    void SwitchToGenericABP();

private:
    void SwitchAnimationBlueprint(TSubclassOf<UAnimInstance> NewAnimBP);
    void SetWeaponState(EWeaponState NewState);
    USkeletalMeshComponent* GetBodyMesh() const;

    void SetIsWeaponDrawnInABP(bool bDrawn);
    void SetWeaponStateInABP(EWeaponState State);

    void OnDrawMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void OnSheathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // Smooth ABP switching with blend
    void SwitchAnimationBlueprintWithBlend(TSubclassOf<UAnimInstance> NewAnimBP, float BlendTime);
    
    // Helper to perform actual ABP switch after blend completes
    void PerformABPSwitch(USkeletalMeshComponent* BodyMesh, TSubclassOf<UAnimInstance> NewAnimBP);
    
    // Switch to generic ABP with blend
    void SwitchToGenericABPWithBlend();
};