#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DBAIStateComponent.generated.h"

/**
 * Universal component for managing gameplay tag states
 * Can be used on ANY actor (Player, Enemy, NPC, Objects, etc.)
 * Automatically handles tag cleanup within tag categories
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBAIStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDBAIStateComponent();

    // ========================================
    // UNIVERSAL TAG MANAGEMENT
    // ========================================

    /**
     * Set a tag, automatically removing other tags in the same category
     * Example: SetTag("State.AI.Idle") removes State.AI.Patrolling, State.AI.Engaged, etc.
     * @param TagName The tag to set (e.g., "State.AI.Idle")
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void SetTag(FName TagName);

    /**
     * Set a tag using FGameplayTag
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void SetGameplayTag(FGameplayTag Tag);

    /**
     * Add a tag without removing siblings (for tags that can coexist)
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void AddTag(FName TagName);

    /**
     * Add a tag using FGameplayTag (for tags that can coexist)
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void AddGameplayTag(FGameplayTag Tag);

    /**
     * Remove a specific tag
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void RemoveTag(FName TagName);

    /**
     * Remove a specific tag using FGameplayTag
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void RemoveGameplayTag(FGameplayTag Tag);

    /**
     * Remove all tags in a category
     * Example: RemoveTagsInCategory("State.AI") removes all State.AI.* tags
     */
    UFUNCTION(BlueprintCallable, Category = "Tag State")
    void RemoveTagsInCategory(FName CategoryTag);

    /**
     * Check if actor has a specific tag
     */
    UFUNCTION(BlueprintPure, Category = "Tag State")
    bool HasTag(FName TagName) const;

    /**
     * Get current tag in a category (e.g., what AI state are we in?)
     */
    UFUNCTION(BlueprintPure, Category = "Tag State")
    FGameplayTag GetCurrentTagInCategory(FName CategoryTag) const;

    // ========================================
    // AI STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Idle() { SetTag(FName("State.AI.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Patrolling() { SetTag(FName("State.AI.Patrolling")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Engaged() { SetTag(FName("State.AI.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Casting() { SetTag(FName("State.AI.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Pursuing() { SetTag(FName("State.AI.Pursuing")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Returning() { SetTag(FName("State.AI.Returning")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Dead() { SetTag(FName("State.AI.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void SetAI_Disabled() { SetTag(FName("State.AI.Disabled")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Idle() { AddTag(FName("State.AI.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Patrolling() { AddTag(FName("State.AI.Patrolling")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Engaged() { AddTag(FName("State.AI.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Casting() { AddTag(FName("State.AI.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Pursuing() { AddTag(FName("State.AI.Pursuing")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Returning() { AddTag(FName("State.AI.Returning")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Dead() { AddTag(FName("State.AI.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void AddAI_Disabled() { AddTag(FName("State.AI.Disabled")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Idle() { RemoveTag(FName("State.AI.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Patrolling() { RemoveTag(FName("State.AI.Patrolling")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Engaged() { RemoveTag(FName("State.AI.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Casting() { RemoveTag(FName("State.AI.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Pursuing() { RemoveTag(FName("State.AI.Pursuing")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Returning() { RemoveTag(FName("State.AI.Returning")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Dead() { RemoveTag(FName("State.AI.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|AI")
    void RemoveAI_Disabled() { RemoveTag(FName("State.AI.Disabled")); }

    // ========================================
    // COMBAT STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void SetCombat_Idle() { SetTag(FName("State.Combat.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void SetCombat_Engaged() { SetTag(FName("State.Combat.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void SetCombat_AutoAttacking() { AddTag(FName("State.Combat.AutoAttacking")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void SetCombat_Casting() { AddTag(FName("State.Combat.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void SetCombat_Blocking() { AddTag(FName("State.Combat.Blocking")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void AddCombat_Idle() { AddTag(FName("State.Combat.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void AddCombat_Engaged() { AddTag(FName("State.Combat.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void AddCombat_AutoAttacking() { AddTag(FName("State.Combat.AutoAttacking")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void AddCombat_Casting() { AddTag(FName("State.Combat.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void AddCombat_Blocking() { AddTag(FName("State.Combat.Blocking")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void RemoveCombat_Idle() { RemoveTag(FName("State.Combat.Idle")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void RemoveCombat_Engaged() { RemoveTag(FName("State.Combat.Engaged")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void RemoveCombat_AutoAttacking() { RemoveTag(FName("State.Combat.AutoAttacking")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void RemoveCombat_Casting() { RemoveTag(FName("State.Combat.Casting")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Combat")
    void RemoveCombat_Blocking() { RemoveTag(FName("State.Combat.Blocking")); }

    // ========================================
    // WEAPON STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void SetWeapon_Drawn() { SetTag(FName("Weapon.Drawn")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void SetWeapon_Sheathed() { SetTag(FName("Weapon.Sheathed")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void AddWeapon_Drawn() { AddTag(FName("Weapon.Drawn")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void AddWeapon_Sheathed() { AddTag(FName("Weapon.Sheathed")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void RemoveWeapon_Drawn() { RemoveTag(FName("Weapon.Drawn")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Weapon")
    void RemoveWeapon_Sheathed() { RemoveTag(FName("Weapon.Sheathed")); }

    // ========================================
    // CHARACTER STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void SetCharacter_Alive() { SetTag(FName("State.Character.Alive")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void SetCharacter_Dead() { SetTag(FName("State.Character.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void AddCharacter_Alive() { AddTag(FName("State.Character.Alive")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void AddCharacter_Dead() { AddTag(FName("State.Character.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void RemoveCharacter_Alive() { RemoveTag(FName("State.Character.Alive")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Character")
    void RemoveCharacter_Dead() { RemoveTag(FName("State.Character.Dead")); }

    // ========================================
    // DEBUFF STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void AddDebuff_Stun() { AddTag(FName("State.Debuff.Stun")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void AddDebuff_Root() { AddTag(FName("State.Debuff.Root")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void AddDebuff_Silence() { AddTag(FName("State.Debuff.Silence")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void AddDebuff_Blind() { AddTag(FName("State.Debuff.Blind")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void RemoveDebuff_Stun() { RemoveTag(FName("State.Debuff.Stun")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void RemoveDebuff_Root() { RemoveTag(FName("State.Debuff.Root")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void RemoveDebuff_Silence() { RemoveTag(FName("State.Debuff.Silence")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Debuff")
    void RemoveDebuff_Blind() { RemoveTag(FName("State.Debuff.Blind")); }

    // ========================================
    // STATUS STATE TAGS (for interrupts)
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void AddStatus_Invulnerable() { AddTag(FName("State.Status.Invulnerable")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void AddStatus_Stunned() { AddTag(FName("State.Status.Stunned")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void AddStatus_Silenced() { AddTag(FName("State.Status.Silenced")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void RemoveStatus_Invulnerable() { RemoveTag(FName("State.Status.Invulnerable")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void RemoveStatus_Stunned() { RemoveTag(FName("State.Status.Stunned")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|Status")
    void RemoveStatus_Silenced() { RemoveTag(FName("State.Status.Silenced")); }

    // ========================================
    // GENERAL STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_ForcedTarget() { AddTag(FName("State.ForcedTarget")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_Counter100() { AddTag(FName("State.Counter100")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_ActiveAbility() { AddTag(FName("State.ActiveAbility")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_Staggered() { AddTag(FName("State.Staggered")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_ActionLock() { AddTag(FName("State.ActionLock")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void AddState_Dead() { AddTag(FName("State.Dead")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_ForcedTarget() { RemoveTag(FName("State.ForcedTarget")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_Counter100() { RemoveTag(FName("State.Counter100")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_ActiveAbility() { RemoveTag(FName("State.ActiveAbility")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_Staggered() { RemoveTag(FName("State.Staggered")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_ActionLock() { RemoveTag(FName("State.ActionLock")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|General")
    void RemoveState_Dead() { RemoveTag(FName("State.Dead")); }

    // ========================================
    // LIGHT MAGE STATE TAGS
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_HolySeal() { AddTag(FName("State.HolySeal")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_EssenceOfPurity() { AddTag(FName("State.EssenceOfPurity")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_LightstepGrace() { AddTag(FName("State.LightstepGrace")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_DeathPrevention() { AddTag(FName("State.DeathPrevention")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_SacredRegen() { AddTag(FName("State.SacredRegen")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void AddState_WarpProtection() { AddTag(FName("State.WarpProtection")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_HolySeal() { RemoveTag(FName("State.HolySeal")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_EssenceOfPurity() { RemoveTag(FName("State.EssenceOfPurity")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_LightstepGrace() { RemoveTag(FName("State.LightstepGrace")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_DeathPrevention() { RemoveTag(FName("State.DeathPrevention")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_SacredRegen() { RemoveTag(FName("State.SacredRegen")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|LightMage")
    void RemoveState_WarpProtection() { RemoveTag(FName("State.WarpProtection")); }

    // ========================================
    // MYSTIC BOON RANKS (Light Mage Passive)
    // ========================================

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void SetMysticBoon_Rank1() { SetTag(FName("State.MysticBoon.Rank1")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void SetMysticBoon_Rank2() { SetTag(FName("State.MysticBoon.Rank2")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void SetMysticBoon_Rank3() { SetTag(FName("State.MysticBoon.Rank3")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void SetMysticBoon_Rank4() { SetTag(FName("State.MysticBoon.Rank4")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void SetMysticBoon_Rank5() { SetTag(FName("State.MysticBoon.Rank5")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void RemoveMysticBoon_Rank1() { RemoveTag(FName("State.MysticBoon.Rank1")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void RemoveMysticBoon_Rank2() { RemoveTag(FName("State.MysticBoon.Rank2")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void RemoveMysticBoon_Rank3() { RemoveTag(FName("State.MysticBoon.Rank3")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void RemoveMysticBoon_Rank4() { RemoveTag(FName("State.MysticBoon.Rank4")); }

    UFUNCTION(BlueprintCallable, Category = "Tag State|MysticBoon")
    void RemoveMysticBoon_Rank5() { RemoveTag(FName("State.MysticBoon.Rank5")); }

protected:
    virtual void BeginPlay() override;

private:
    /** Reference to owner's Ability System Component */
    UPROPERTY()
    class UAbilitySystemComponent* ASC;

    /** Get the parent tag from a full tag path (e.g., "State.AI" from "State.AI.Idle") */
    FGameplayTag GetParentTag(const FGameplayTag& Tag) const;
};