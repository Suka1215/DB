// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "DBCharacterBase.h"
#include "GameplayAbilitySpecHandle.h"
#include "DBPlayerCharacter.generated.h"

class UDBJobComponent;
class UDBLoadoutComponent;
class UAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class DAWNBLADE_API ADBPlayerCharacter : public ADBCharacterBase
{
    GENERATED_BODY()

public:
    ADBPlayerCharacter();

    /** IAbilitySystemInterface */
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    /** Called on server when possessed; client mirrors via OnRep_PlayerState */
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    /** Click-to-activate (used by debug UI) */
    UFUNCTION(BlueprintCallable, Category="Abilities")
    bool ActivateAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass);

    /** Build a "spellbook" list for UI (job grants + learned) */
    UFUNCTION(BlueprintCallable, Category="Abilities")
    void GetKnownAbilities(TArray<TSubclassOf<class UGameplayAbility>>& OutAbilities) const;

protected:
    // Engine
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UDBJobComponent* Job;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UDBLoadoutComponent* Loadout;

    // Camera Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    float BaseTurnRate = 45.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    float BaseLookUpRate = 45.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    float GamepadSensitivity = 1.0f;

    // Enhanced Input - Input Mapping Context (CRITICAL - set this in Blueprint!)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputMappingContext* DefaultMappingContext;

    // Enhanced Input Actions (assign in BP)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* PrimaryAAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* PrimaryBAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* PrimaryXAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
    UInputAction* PrimaryYAction;

    // Movement
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    // Ability activations (gamepad face buttons)
    void UsePrimaryA();
    void UsePrimaryB();
    void UsePrimaryX();
    void UsePrimaryY();

    // Activate by "slot" (reads Loadout ActiveSet -> Entries[Slot].Ability, finds spec in ASC, calls TryActivate)
    void TryActivateBySlot(int32 SlotIndex);

    // ===== Ability grant pipeline =====
    /** Main sync: compute desired abilities and grant/remove diffs. (SERVER ONLY) */
    UFUNCTION()
    void HandleActiveSetChanged(int32 NewIndex);

    UFUNCTION()
    void HandleJobChanged();

    UFUNCTION()
    void HandleJobLevelChanged();
    
    UFUNCTION()
    void SyncGrantedAbilities();

    UFUNCTION()
    void HandleActiveGrantsChanged();

    /** Has a spec of this class already? */
    static bool ASC_HasAbilityClass(class UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass);

    /** GiveAbility safely (server only) */
    static void ASC_GiveAbilityIfMissing(class UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass, int32 Level = 1);

    /** Handles we granted (so we can retract them later if no longer desired) */
    TMap<TSubclassOf<class UGameplayAbility>, FGameplayAbilitySpecHandle> GrantedBySystem;

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};