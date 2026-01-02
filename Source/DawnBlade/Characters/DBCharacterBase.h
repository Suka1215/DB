// DBCharacterBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "DBCharacterBase.generated.h"

class UAbilitySystemComponent;
class UDBAttr_Primary;
class UDBAttr_Combat;
class UDBAttr_Resistance;
class UGameplayAbility;
class UGameplayEffect;
struct FOnAttributeChangeData;

/** Delegate for health changes - bind UI here */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);

/** Delegate for mana changes - bind UI here */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaChangedSignature, float, NewMana, float, MaxMana);

/** Delegate for character death */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ADBCharacterBase*, DeadCharacter);

/**
 * Base character class for all characters in DawnBlade.
 * Provides shared GAS, attributes, and core functionality.
 * 
 * Hierarchy:
 *   ACharacter (Engine)
 *     └── ADBCharacterBase (this class)
 *           ├── ADBPlayerCharacter (Player)
 *           └── ADBEnemyCharacter (Enemy)
 *                 └── BP_Enemy_Goblin (Blueprint)
 */
UCLASS(Abstract)
class DAWNBLADE_API ADBCharacterBase : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADBCharacterBase();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ========================================
    // IAbilitySystemInterface
    // ========================================

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // ========================================
    // DELEGATES
    // ========================================

    /** Broadcast when health changes */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChangedSignature OnHealthChanged;

    /** Broadcast when mana changes */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnManaChangedSignature OnManaChanged;

    /** Broadcast when character dies */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCharacterDeath OnCharacterDied;

    // ========================================
    // COMPONENTS
    // ========================================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UDBAttr_Primary> PrimaryAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UDBAttr_Combat> CombatAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UDBAttr_Resistance> ResistanceAttributes;

    // ========================================
    // PUBLIC GETTERS
    // ========================================

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetMana() const;

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetMaxMana() const;

    UFUNCTION(BlueprintPure, Category = "Character|Attributes")
    float GetManaPercent() const;

    UFUNCTION(BlueprintPure, Category = "Character")
    int32 GetCharacterLevel() const;

    UFUNCTION(BlueprintPure, Category = "Character")
    bool IsDead() const { return bIsDead; }

    // ========================================
    // ABILITY HELPERS
    // ========================================

    /** Grant an ability to this character */
    UFUNCTION(BlueprintCallable, Category = "Character|Abilities")
    void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);

    /** Apply a gameplay effect to this character */
    UFUNCTION(BlueprintCallable, Category = "Character|Abilities")
    void ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass, int32 Level = 1);

    /** Check if character has a gameplay tag */
    UFUNCTION(BlueprintPure, Category = "Character|Tags")
    bool HasGameplayTag(FGameplayTag Tag) const;

    /** Add a loose gameplay tag */
    UFUNCTION(BlueprintCallable, Category = "Character|Tags")
    void AddGameplayTag(FGameplayTag Tag);

    /** Remove a loose gameplay tag */
    UFUNCTION(BlueprintCallable, Category = "Character|Tags")
    void RemoveGameplayTag(FGameplayTag Tag);

protected:
    // ========================================
    // INITIALIZATION - Override in subclasses
    // ========================================

    /** Initialize GAS actor info - called in BeginPlay */
    virtual void InitAbilityActorInfo();

    /** Apply default stats - SERVER ONLY, override in subclasses */
    virtual void InitClassDefaults();

    /** Bind to attribute change delegates */
    virtual void BindCallbacksToDependencies();

    /** Broadcast initial attribute values to UI/clients */
    virtual void BroadcastInitialValues();

    // ========================================
    // ATTRIBUTE CHANGE HANDLERS
    // ========================================

    /** Called when health attribute changes */
    virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

    /** Called when mana attribute changes */
    virtual void OnManaAttributeChanged(const FOnAttributeChangeData& Data);

    // ========================================
    // DEATH - Override in subclasses for custom behavior
    // ========================================

    /** Handle death - SERVER ONLY */
    UFUNCTION(BlueprintCallable, Category = "Character")
    virtual void Death();

    // ========================================
    // STATE
    // ========================================

    /** Is this character dead? */
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    bool bIsDead = false;

    /** Replicated flag indicating attributes are initialized */
    UPROPERTY(ReplicatedUsing = OnRep_InitAttributes)
    bool bInitAttributes = false;

    UFUNCTION()
    virtual void OnRep_InitAttributes();
};