#include "Components/DBCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

UDBCombatComponent::UDBCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
    SetIsReplicatedByDefault(true);
    
    TargetIndicatorWidget = nullptr;
}

void UDBCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get ASC from owner
    if (AActor* Owner = GetOwner())
    {
        if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
        {
            AbilitySystemComponent = ASI->GetAbilitySystemComponent();
        }
    }
}

void UDBCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsInCombat())
    {
        TimeSinceLastTargetCheck += DeltaTime;
        if (TimeSinceLastTargetCheck >= TargetCheckInterval)
        {
            CheckTargetValidity();
            TimeSinceLastTargetCheck = 0.f;
        }
    }
}

void UDBCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UDBCombatComponent, CurrentTarget);
}

void UDBCombatComponent::SetTarget(AActor* NewTarget)
{
    if (CurrentTarget != NewTarget)
    {
        CurrentTarget = NewTarget;
        OnTargetChanged.Broadcast(NewTarget);
        
        // Update target indicator
        UpdateTargetIndicator();
    }
}

void UDBCombatComponent::UpdateTargetIndicator()
{
    // Remove old indicator
    if (TargetIndicatorWidget)
    {
        TargetIndicatorWidget->DestroyComponent();
        TargetIndicatorWidget = nullptr;
    }

    // Create new indicator if we have a target and widget class
    if (CurrentTarget && TargetIndicatorClass)
    {
        // Create widget component
        TargetIndicatorWidget = NewObject<UWidgetComponent>(CurrentTarget);
        TargetIndicatorWidget->SetWidgetClass(TargetIndicatorClass);
        TargetIndicatorWidget->SetWidgetSpace(EWidgetSpace::Screen);
        TargetIndicatorWidget->SetDrawSize(FVector2D(64.0f, 64.0f));
        TargetIndicatorWidget->RegisterComponent();
        
        // Attach above target's head
        if (ACharacter* TargetChar = Cast<ACharacter>(CurrentTarget))
        {
            TargetIndicatorWidget->AttachToComponent(
                TargetChar->GetRootComponent(),
                FAttachmentTransformRules::SnapToTargetIncludingScale
            );
            TargetIndicatorWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
        }
        
        UE_LOG(LogTemp, Warning, TEXT("🎯 Target indicator placed on: %s"), *CurrentTarget->GetName());
    }
}

void UDBCombatComponent::OnRep_CurrentTarget()
{
    OnTargetChanged.Broadcast(CurrentTarget);
    UpdateTargetIndicator();
}

void UDBCombatComponent::ServerEngageTarget_Implementation(AActor* Target)
{
    if (!Target || !AbilitySystemComponent) return;

    SetTarget(Target);

    // Trigger DBGA_Engage ability by tag
    FGameplayTag EngageAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Engage"));
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(EngageAbilityTag);

    // This will call your DBGA_Engage which handles the montage and Weapon.Drawn tag
    bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
    
    if (bActivated)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Engage ability activated for target: %s"), *Target->GetName());
        
        // Apply combat engaged tag
        FGameplayTag EngagedTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.Engaged"));
        AbilitySystemComponent->AddLooseGameplayTag(EngagedTag);

        // Start auto-attack after a delay (let engage montage finish)
        FTimerHandle DelayTimer;
        GetWorld()->GetTimerManager().SetTimer(DelayTimer, [this]()
        {
            StartAutoAttack();
        }, 1.5f, false);

        OnCombatStateChanged.Broadcast();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to activate Engage ability!"));
    }
}

void UDBCombatComponent::ServerDisengage_Implementation()
{
    if (!AbilitySystemComponent) return;

    // Stop auto-attack
    StopAutoAttack();

    // Trigger DBGA_Disengage ability by tag (if you have one)
    FGameplayTag DisengageAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Disengage"));
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(DisengageAbilityTag);
    AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);

    // Remove engaged tag
    FGameplayTag EngagedTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.Engaged"));
    AbilitySystemComponent->RemoveLooseGameplayTag(EngagedTag);

    SetTarget(nullptr);
    OnCombatStateChanged.Broadcast();
    
    UE_LOG(LogTemp, Warning, TEXT("🏃 Disengaged from combat"));
}

bool UDBCombatComponent::IsInCombat() const
{
    if (!AbilitySystemComponent) return false;

    FGameplayTag EngagedTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.Engaged"));
    return AbilitySystemComponent->HasMatchingGameplayTag(EngagedTag);
}

bool UDBCombatComponent::IsAutoAttacking() const
{
    if (!AbilitySystemComponent) return false;

    FGameplayTag AutoAttackTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.AutoAttacking"));
    return AbilitySystemComponent->HasMatchingGameplayTag(AutoAttackTag);
}

void UDBCombatComponent::CheckTargetValidity()
{
    if (!CurrentTarget)
    {
        ServerDisengage();
        return;
    }

    // Check if target is dead
    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentTarget);
    if (TargetASC)
    {
        FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
        if (TargetASC->HasMatchingGameplayTag(DeadTag))
        {
            UE_LOG(LogTemp, Warning, TEXT("💀 Target died - disengaging"));
            ServerDisengage();
            return;
        }
    }

    // Check distance
    AActor* Owner = GetOwner();
    if (Owner)
    {
        float Distance = FVector::Dist(Owner->GetActorLocation(), CurrentTarget->GetActorLocation());
        if (Distance > AutoDisengageDistance)
        {
            UE_LOG(LogTemp, Warning, TEXT("🏃 Target too far (%.1fm) - disengaging"), Distance / 100.0f);
            ServerDisengage();
            return;
        }
    }
}

void UDBCombatComponent::StartAutoAttack()
{
    if (!AbilitySystemComponent) return;

    // Try to activate auto-attack ability by tag
    FGameplayTag AutoAttackAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.AutoAttack"));
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(AutoAttackAbilityTag);

    bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
    
    if (bActivated)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚔️ Auto-attack started"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Auto-attack ability not found or couldn't activate"));
    }
}

void UDBCombatComponent::StopAutoAttack()
{
    if (!AbilitySystemComponent) return;

    // Cancel abilities with AutoAttacking state tag
    FGameplayTag AutoAttackingTag = FGameplayTag::RequestGameplayTag(FName("State.Combat.AutoAttacking"));
    FGameplayTagContainer CancelTags;
    CancelTags.AddTag(AutoAttackingTag);
    AbilitySystemComponent->CancelAbilities(&CancelTags);
    
    UE_LOG(LogTemp, Warning, TEXT("🛑 Auto-attack stopped"));
}