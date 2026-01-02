// DBGA_Engage.cpp - Corrected for Single ABP with Job State Machines
#include "Abilities/Combat/DBGA_Engage.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"

UDBGA_Engage::UDBGA_Engage()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Action.Engage")));
    CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Action")));
    
    // Montage is now assigned via Blueprint or will be null
    // TODO: Assign EngageMontage in Blueprint defaults or fix asset path
    EngageMontage = nullptr;
}

void UDBGA_Engage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ ENGAGE: Failed to commit ability"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ ENGAGE: No character"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("⚔️ ========== ENGAGE ABILITY ACTIVATED =========="));
    
    // ========================================
    // STEP 1: Add Weapon.Drawn tag
    // This triggers the ABP to switch to combat stance
    // ========================================
    FGameplayTagContainer TagsToAdd;
    TagsToAdd.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn")));
    
    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        ASC->AddLooseGameplayTags(TagsToAdd);
        UE_LOG(LogTemp, Warning, TEXT("✅ ENGAGE: Added Weapon.Drawn tag"));
        
        // Verify tag was added
        if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn"))))
        {
            UE_LOG(LogTemp, Warning, TEXT("✅ ENGAGE: Tag verified present"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ ENGAGE: Tag NOT found after adding!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ ENGAGE: ASC is NULL!"));
    }
    
    // ========================================
    // STEP 2: Play engage montage (if available)
    // ========================================
    if (!EngageMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ ENGAGE: No montage assigned - completing immediately"));
        EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🎬 ENGAGE: Playing montage: %s"), *EngageMontage->GetName());
    UE_LOG(LogTemp, Warning, TEXT("🎬 ENGAGE: Montage path: %s"), *EngageMontage->GetPathName());
    
    UAbilityTask_PlayMontageAndWait* MontageTask = 
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            EngageMontage,
            1.0f,
            NAME_None,
            true,
            1.0f
        );
    
    MontageTask->OnCompleted.AddDynamic(this, &UDBGA_Engage::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &UDBGA_Engage::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UDBGA_Engage::OnMontageCancelled);
    
    MontageTask->ReadyForActivation();
}

void UDBGA_Engage::OnMontageCompleted()
{
    UE_LOG(LogTemp, Warning, TEXT("✅ ENGAGE: Montage completed successfully"));
    UE_LOG(LogTemp, Warning, TEXT("⚔️ ========== ENGAGE COMPLETE =========="));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UDBGA_Engage::OnMontageInterrupted()
{
    UE_LOG(LogTemp, Warning, TEXT("⚠️ ENGAGE: Montage interrupted"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDBGA_Engage::OnMontageCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("⚠️ ENGAGE: Montage cancelled"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDBGA_Engage::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    // NOTE: We do NOT remove the Weapon.Drawn tag here!
    // It should stay until Disengage is called
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}