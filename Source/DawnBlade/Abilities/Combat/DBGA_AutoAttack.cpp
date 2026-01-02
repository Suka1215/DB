// DBGA_AutoAttack.cpp

#include "Abilities/Combat/DBGA_AutoAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/DBCombatComponent.h"
#include "Attributes/DBAttr_Combat.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

UDBGA_AutoAttack::UDBGA_AutoAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

    // FIXED: Use new API for setting tags
    FGameplayTagContainer Tags;
    Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.AutoAttack")));
    SetAssetTags(Tags);
    
    // Can't activate while dead or casting spells
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Combat.Casting")));
    
    // Cancel other auto-attacks if somehow activated again
    CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.AutoAttack")));
}

void UDBGA_AutoAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Apply auto-attacking tag
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Combat.AutoAttacking")));
    GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTags(TagContainer);

    UE_LOG(LogTemp, Log, TEXT("[AutoAttack] Activated - Starting auto-attack loop"));

    // Start the auto-attack loop
    PerformAutoAttack();
}

void UDBGA_AutoAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoAttackTimerHandle);
    }

    // Remove auto-attacking tag
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Combat.AutoAttacking")));
    GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTags(TagContainer);

    UE_LOG(LogTemp, Log, TEXT("[AutoAttack] Ended - Cancelled: %s"), bWasCancelled ? TEXT("Yes") : TEXT("No"));

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDBGA_AutoAttack::PerformAutoAttack()
{
    // Check if target is still valid and in range
    AActor* Target = GetCurrentTarget();
    if (!Target || !IsTargetInRange())
    {
        UE_LOG(LogTemp, Warning, TEXT("[AutoAttack] Target invalid or out of range - ending ability"));
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[AutoAttack] Performing attack on target: %s"), *Target->GetName());

    // Get appropriate attack montage
    UAnimMontage* AttackMontage = nullptr;
    if (AttackMontages.Num() > 0)
    {
        AttackMontage = AttackMontages[CurrentAttackIndex % AttackMontages.Num()];
        CurrentAttackIndex++;
    }

    if (!AttackMontage)
    {
        // No montage - just apply damage directly and schedule next attack
        UE_LOG(LogTemp, Warning, TEXT("[AutoAttack] No attack montage - applying damage directly"));
        ApplyDamageToTarget(Target);
        ScheduleNextAttack();
        return;
    }

    // Play attack montage
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        AttackMontage,
        1.0f,
        NAME_None,
        false,
        1.0f
    );

    Task->OnCompleted.AddDynamic(this, &UDBGA_AutoAttack::OnMontageCompleted);
    Task->OnBlendOut.AddDynamic(this, &UDBGA_AutoAttack::OnMontageBlendOut);
    Task->OnCancelled.AddDynamic(this, &UDBGA_AutoAttack::OnMontageCancelled);
    Task->OnInterrupted.AddDynamic(this, &UDBGA_AutoAttack::OnMontageCancelled);

    Task->ReadyForActivation();

    // Apply damage (animation notify could also trigger this at precise moment)
    ApplyDamageToTarget(Target);
}

void UDBGA_AutoAttack::OnMontageCompleted()
{
    UE_LOG(LogTemp, Log, TEXT("[AutoAttack] Montage completed - scheduling next attack"));
    ScheduleNextAttack();
}

void UDBGA_AutoAttack::OnMontageBlendOut()
{
    // Montage blending out naturally
}

void UDBGA_AutoAttack::OnMontageCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("[AutoAttack] Montage cancelled - ending ability"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDBGA_AutoAttack::ScheduleNextAttack()
{
    float AttackInterval = GetAttackInterval();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            AutoAttackTimerHandle,
            this,
            &UDBGA_AutoAttack::PerformAutoAttack,
            AttackInterval,
            false
        );
    }
}

float UDBGA_AutoAttack::GetAttackInterval() const
{
    // Try to get Attack Speed from combat attributes
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC)
    {
        const UDBAttr_Combat* CombatAttr = ASC->GetSet<UDBAttr_Combat>();
        if (CombatAttr)
        {
            float AttackSpeed = CombatAttr->GetAttack();
            if (AttackSpeed > 0.f)
            {
                // Convert attack power to speed multiplier
                // Higher attack = faster attacks (simplified)
                float SpeedMult = 1.0f + (AttackSpeed / 100.f);
                return BaseAttackInterval / FMath::Max(SpeedMult, 0.5f);
            }
        }
    }

    return BaseAttackInterval;
}

bool UDBGA_AutoAttack::IsTargetInRange() const
{
    AActor* Target = GetCurrentTarget();
    if (!Target) return false;

    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return false;

    float Distance = FVector::Dist(Avatar->GetActorLocation(), Target->GetActorLocation());
    return Distance <= MeleeRange;
}

AActor* UDBGA_AutoAttack::GetCurrentTarget() const
{
    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        if (UDBCombatComponent* CombatComp = Character->FindComponentByClass<UDBCombatComponent>())
        {
            return CombatComp->GetCurrentTarget();
        }
    }
    return nullptr;
}

void UDBGA_AutoAttack::ApplyDamageToTarget(AActor* Target)
{
    if (!Target) return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    if (!TargetASC) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    // Calculate damage based on Attack attribute
    float FinalDamage = BaseDamage;
    
    const UDBAttr_Combat* CombatAttr = SourceASC->GetSet<UDBAttr_Combat>();
    if (CombatAttr)
    {
        FinalDamage += CombatAttr->GetAttack();
    }

    // Create damage effect spec
    if (DamageEffect)
    {
        FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
            DamageEffect,
            GetAbilityLevel(),
            EffectContext
        );

        if (SpecHandle.IsValid())
        {
            // Set damage magnitude
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(FName("SetByCaller.Damage")),
                FinalDamage
            );

            // Apply to target
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

            UE_LOG(LogTemp, Log, TEXT("[AutoAttack] Applied %f damage to %s"), FinalDamage, *Target->GetName());
        }
    }
}