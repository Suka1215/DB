// DBEnemyCharacter.cpp
#include "DBEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"
#include "Attributes/DBAttr_Resistance.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "AI/DBAIController_Enemy.h"
#include "AI/Components/DBAIStateComponent.h"
#include "Data/DEnemyData.h"
#include "Kismet/GameplayStatics.h"

ADBEnemyCharacter::ADBEnemyCharacter()
{
    // AI State Component
    AIStateComponent = CreateDefaultSubobject<UDBAIStateComponent>(TEXT("AIStateComponent"));

    // Health bar widget
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(150.0f, 20.0f));

    // AI Controller
    AIControllerClass = ADBAIController_Enemy::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Tag for targeting
    Tags.Add(FName("Enemy"));
}

void ADBEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set movement speed from data if available
    if (EnemyData)
    {
        if (UCharacterMovementComponent* Movement = GetCharacterMovement())
        {
            Movement->MaxWalkSpeed = EnemyData->MovementSpeed;
        }
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void ADBEnemyCharacter::InitClassDefaults()
{
    // Server only - apply stats from EnemyData
    if (!EnemyData)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ %s has no EnemyData assigned!"), *GetName());
        return;
    }

    // Primary Attributes
    PrimaryAttributes->SetLevel(EnemyData->Level);
    PrimaryAttributes->SetMaxHealth(EnemyData->BaseHealth);
    PrimaryAttributes->SetHealth(EnemyData->BaseHealth);
    PrimaryAttributes->SetMaxMana(EnemyData->BaseMana);
    PrimaryAttributes->SetMana(EnemyData->BaseMana);

    PrimaryAttributes->SetStrength(EnemyData->Strength);
    PrimaryAttributes->SetVitality(EnemyData->Vitality);
    PrimaryAttributes->SetDexterity(EnemyData->Dexterity);
    PrimaryAttributes->SetAgility(EnemyData->Agility);
    PrimaryAttributes->SetIntelligence(EnemyData->Intelligence);
    PrimaryAttributes->SetMind(EnemyData->Mind);
    PrimaryAttributes->SetCharisma(EnemyData->Charisma);

    // Combat Attributes
    CombatAttributes->SetAttack(EnemyData->Attack);
    CombatAttributes->SetDefense(EnemyData->Defense);
    CombatAttributes->SetEvasion(EnemyData->Evasion);
    CombatAttributes->SetAccuracy(EnemyData->Accuracy);

    // Resistances
    ResistanceAttributes->SetFireResistance(EnemyData->FireResistance);
    ResistanceAttributes->SetIceResistance(EnemyData->IceResistance);
    ResistanceAttributes->SetLightningResistance(EnemyData->LightningResistance);
    ResistanceAttributes->SetWaterResistance(EnemyData->WaterResistance);
    ResistanceAttributes->SetLightResistance(EnemyData->LightResistance);
    ResistanceAttributes->SetDarkResistance(EnemyData->DarkResistance);

    // Gameplay Tags
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTags(EnemyData->GameplayTags);

        FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Character.Enemy"));
        AbilitySystemComponent->AddLooseGameplayTag(EnemyTag);
    }

    // Grant Abilities
    for (TSubclassOf<UGameplayAbility> AbilityClass : EnemyData->SpecialAbilities)
    {
        if (AbilityClass)
        {
            GrantAbility(AbilityClass, EnemyData->Level);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ %s initialized: %s (Lvl %d, HP %.0f)"),
        *GetName(),
        *EnemyData->EnemyName.ToString(),
        EnemyData->Level,
        PrimaryAttributes->GetHealth());

    BroadcastInitialValues();
}

// ============================================================================
// DEATH
// ============================================================================

void ADBEnemyCharacter::Death()
{
    if (!HasAuthority()) return;

    // Call base death logic
    Super::Death();

    // Play death sound
    if (EnemyData && EnemyData->DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EnemyData->DeathSound, GetActorLocation());
    }

    // Broadcast enemy-specific death event (for spawner tracking)
    OnEnemyDied.Broadcast(this);

    // Disable collision
    SetActorEnableCollision(false);

    // Destroy after delay
    float RemovalTime = EnemyData ? EnemyData->DeathRemovalTime : 5.0f;
    SetLifeSpan(RemovalTime);
}

// ============================================================================
// COMBAT
// ============================================================================

void ADBEnemyCharacter::SetCurrentTarget(AActor* NewTarget)
{
    CurrentTarget = NewTarget;
}

void ADBEnemyCharacter::ApplyDamageToEnemy(float Damage, AActor* DamageCauser)
{
    if (!HasAuthority() || !PrimaryAttributes || bIsDead) return;

    float NewHealth = FMath::Max(0.0f, PrimaryAttributes->GetHealth() - Damage);
    PrimaryAttributes->SetHealth(NewHealth);

    // Play hit sound
    if (EnemyData && EnemyData->HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EnemyData->HitSound, GetActorLocation());
    }
}

void ADBEnemyCharacter::PerformAttack(const FEnemyAttackPattern& AttackPattern)
{
    if (!AttackPattern.AttackMontage || bIsAttacking || bIsDead) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    bIsAttacking = true;

    // Bind montage end delegate
    MontageEndedDelegate.BindUObject(this, &ADBEnemyCharacter::OnAttackMontageEnded);

    float Duration = AnimInstance->Montage_Play(AttackPattern.AttackMontage, 1.0f);

    if (Duration > 0.0f)
    {
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackPattern.AttackMontage);

        if (AttackPattern.AttackSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, AttackPattern.AttackSound, GetActorLocation());
        }

        UE_LOG(LogTemp, Log, TEXT("⚔️ %s performing attack: %s"),
            *GetName(), *AttackPattern.AttackName.ToString());
    }
    else
    {
        bIsAttacking = false;
    }
}

void ADBEnemyCharacter::MulticastPlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
    if (!Montage) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    AnimInstance->Montage_Play(Montage, PlayRate);
}

void ADBEnemyCharacter::PerformAutoAttack()
{
    if (!EnemyData || bIsAttacking || bIsDead) return;

    if (EnemyData->AttackPatterns.Num() > 0)
    {
        FEnemyAttackPattern Attack = EnemyData->GetRandomAttackPattern();
        PerformAttack(Attack);
    }
}

void ADBEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsAttacking = false;
    OnAttackCompleted.ExecuteIfBound();
}