#include "Abilities/Combat/DBGA_AutoEngage.h"
#include "Components/DBCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UDBGA_AutoEngage::UDBGA_AutoEngage()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    
    // This ability should always be active
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Passive.AutoEngage")));
    
    LastDetectedTarget = nullptr;
    bHasEngaged = false;
}

void UDBGA_AutoEngage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("🎯 Auto-Engage system activated"));

    // Start checking for enemies
    GetWorld()->GetTimerManager().SetTimer(
        CheckTimer,
        this,
        &UDBGA_AutoEngage::CheckForEnemies,
        CheckInterval,
        true
    );

    // Do first check immediately
    CheckForEnemies();
}

void UDBGA_AutoEngage::EndAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    GetWorld()->GetTimerManager().ClearTimer(CheckTimer);
    LastDetectedTarget = nullptr;
    bHasEngaged = false;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDBGA_AutoEngage::CheckForEnemies()
{
    ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (!Character)
    {
        return;
    }

    UDBCombatComponent* CombatComp = Character->FindComponentByClass<UDBCombatComponent>();
    if (!CombatComp)
    {
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        return;
    }

    // Find all enemies with "Enemy" tag
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundActors);

    // Find nearest enemy
    AActor* NearestEnemy = nullptr;
    float NearestDistance = DetectionRange;
    FVector MyLocation = Character->GetActorLocation();

    for (AActor* Actor : FoundActors)
    {
        if (!Actor || Actor == Character)
        {
            continue;
        }

        float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
        
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestEnemy = Actor;
        }
    }

    // Handle target detection
    if (NearestEnemy)
    {
        // New target detected
        if (LastDetectedTarget != NearestEnemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("🎯 Enemy detected: %s (%.1fm away)"), 
                *NearestEnemy->GetName(), NearestDistance / 100.0f);
            
            LastDetectedTarget = NearestEnemy;
            
            // Set as current target (shows indicator)
            CombatComp->SetTarget(NearestEnemy);
        }

        // Check if in engage range
        if (NearestDistance <= EngageRange)
        {
            // Check if already engaged
            bool bAlreadyEngaged = ASC->HasMatchingGameplayTag(
                FGameplayTag::RequestGameplayTag(FName("State.Combat.Engaged")));

            if (!bAlreadyEngaged)
            {
                UE_LOG(LogTemp, Warning, TEXT("⚔️ Enemy in engage range (%.1fm) - engaging!"), 
                    NearestDistance / 100.0f);
                
                // Call the server to engage
                CombatComp->ServerEngageTarget(NearestEnemy);
                
                bHasEngaged = true;
            }
        }
    }
    else
    {
        // No enemies in detection range
        if (LastDetectedTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("👁️ Lost sight of enemy"));
            LastDetectedTarget = nullptr;
            CombatComp->SetTarget(nullptr);
        }
    }
}