// ============================================================================
// DBGA_Disengage.cpp - WITH ABP Switching Back to Generic
// ============================================================================

#include "Abilities/Combat/DBGA_Disengage.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "DawnBlade/Components/DBJobComponent.h"

UDBGA_Disengage::UDBGA_Disengage()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Action.Disengage")));
    CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Action")));
    
    // Montage is now assigned via Blueprint or will be null
    // TODO: Assign DisengageMontage in Blueprint defaults or fix asset path
    DisengageMontage = nullptr;
}

void UDBGA_Disengage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🛡️ Disengaging - Sheathing weapons..."));
    
    // ========================================
    // STEP 1: Play disengage montage (if available)
    // ========================================
    if (!DisengageMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No disengage montage assigned - completing immediately"));
        
        // Still do cleanup even without montage
        if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
        {
            FGameplayTagContainer TagsToRemove;
            TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn")));
            ASC->RemoveLooseGameplayTags(TagsToRemove);
            UE_LOG(LogTemp, Warning, TEXT("✅ Removed Weapon.Drawn tag"));
        }
        
        if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
        {
            if (UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>())
            {
                JobComp->SetAnimationBlueprintForJob(JobComp->GetCurrentJob(), false);
            }
        }
        
        EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🎬 Playing disengage montage"));
    
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        DisengageMontage,
        1.0f,
        NAME_None,
        true,
        1.0f
    );
    
    Task->OnCompleted.AddDynamic(this, &UDBGA_Disengage::OnMontageCompleted);
    Task->OnInterrupted.AddDynamic(this, &UDBGA_Disengage::OnMontageInterrupted);
    Task->OnCancelled.AddDynamic(this, &UDBGA_Disengage::OnMontageCancelled);
    
    Task->ReadyForActivation();
}

void UDBGA_Disengage::OnMontageCompleted()
{
    UE_LOG(LogTemp, Warning, TEXT("✅ Disengage montage completed"));
    
    // ========================================
    // STEP 2: Remove Weapon.Drawn tag
    // ========================================
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        FGameplayTagContainer TagsToRemove;
        TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn")));
        ASC->RemoveLooseGameplayTags(TagsToRemove);
        UE_LOG(LogTemp, Warning, TEXT("✅ Removed Weapon.Drawn tag"));
    }
    
    // ========================================
    // STEP 3: Switch back to generic ABP
    // ========================================
    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        if (UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>())
        {
            JobComp->SetAnimationBlueprintForJob(JobComp->GetCurrentJob(), false);
            UE_LOG(LogTemp, Warning, TEXT("🎭 Switched to generic ABP - back to normal stance"));
        }
    }
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UDBGA_Disengage::OnMontageInterrupted()
{
    UE_LOG(LogTemp, Warning, TEXT("⚠️ Disengage interrupted - cleaning up"));
    
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        FGameplayTagContainer TagsToRemove;
        TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn")));
        ASC->RemoveLooseGameplayTags(TagsToRemove);
    }
    
    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        if (UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>())
        {
            JobComp->SetAnimationBlueprintForJob(JobComp->GetCurrentJob(), false);
        }
    }
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDBGA_Disengage::OnMontageCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("⚠️ Disengage cancelled - cleaning up"));
    
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        FGameplayTagContainer TagsToRemove;
        TagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn")));
        ASC->RemoveLooseGameplayTags(TagsToRemove);
    }
    
    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        if (UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>())
        {
            JobComp->SetAnimationBlueprintForJob(JobComp->GetCurrentJob(), false);
        }
    }
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDBGA_Disengage::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}