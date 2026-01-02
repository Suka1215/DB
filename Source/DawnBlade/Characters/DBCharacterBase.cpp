// DBCharacterBase.cpp
#include "DBCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"
#include "Attributes/DBAttr_Resistance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ADBCharacterBase::ADBCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // Replication
    bReplicates = true;
    SetReplicateMovement(true);

    // Rotation settings - let movement handle rotation
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Attributes
    PrimaryAttributes = CreateDefaultSubobject<UDBAttr_Primary>(TEXT("PrimaryAttributes"));
    CombatAttributes = CreateDefaultSubobject<UDBAttr_Combat>(TEXT("CombatAttributes"));
    ResistanceAttributes = CreateDefaultSubobject<UDBAttr_Resistance>(TEXT("ResistanceAttributes"));

    // Movement defaults
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->bOrientRotationToMovement = true;
        Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    }
}

void ADBCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    BindCallbacksToDependencies();
    InitAbilityActorInfo();
}

void ADBCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADBCharacterBase, bInitAttributes);
}

// ============================================================================
// IAbilitySystemInterface
// ============================================================================

UAbilitySystemComponent* ADBCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void ADBCharacterBase::InitAbilityActorInfo()
{
    if (AbilitySystemComponent && PrimaryAttributes)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        if (HasAuthority())
        {
            InitClassDefaults();
            bInitAttributes = true;
        }
    }
}

void ADBCharacterBase::InitClassDefaults()
{
    // Override in subclasses to set default stats
    // e.g., from EnemyData or PlayerData
}

void ADBCharacterBase::BindCallbacksToDependencies()
{
    if (AbilitySystemComponent && PrimaryAttributes)
    {
        // Health changes
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            PrimaryAttributes->GetHealthAttribute()).AddUObject(
                this, &ADBCharacterBase::OnHealthAttributeChanged);

        // Mana changes
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            PrimaryAttributes->GetManaAttribute()).AddUObject(
                this, &ADBCharacterBase::OnManaAttributeChanged);
    }
}

void ADBCharacterBase::BroadcastInitialValues()
{
    if (PrimaryAttributes)
    {
        OnHealthChanged.Broadcast(PrimaryAttributes->GetHealth(), PrimaryAttributes->GetMaxHealth());
        OnManaChanged.Broadcast(PrimaryAttributes->GetMana(), PrimaryAttributes->GetMaxMana());
    }
}

void ADBCharacterBase::OnRep_InitAttributes()
{
    BroadcastInitialValues();
}

// ============================================================================
// ATTRIBUTE CHANGE HANDLERS
// ============================================================================

void ADBCharacterBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    OnHealthChanged.Broadcast(Data.NewValue, PrimaryAttributes->GetMaxHealth());

    // Check for death on server
    if (HasAuthority() && Data.NewValue <= 0.0f && !bIsDead)
    {
        Death();
    }
}

void ADBCharacterBase::OnManaAttributeChanged(const FOnAttributeChangeData& Data)
{
    OnManaChanged.Broadcast(Data.NewValue, PrimaryAttributes->GetMaxMana());
}

// ============================================================================
// DEATH
// ============================================================================

void ADBCharacterBase::Death()
{
    if (!HasAuthority()) return;

    bIsDead = true;

    // Add dead tag
    if (AbilitySystemComponent)
    {
        FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
    }

    // Broadcast death
    OnCharacterDied.Broadcast(this);

    UE_LOG(LogTemp, Warning, TEXT("💀 %s died"), *GetName());
}

// ============================================================================
// PUBLIC GETTERS
// ============================================================================

float ADBCharacterBase::GetHealth() const
{
    return PrimaryAttributes ? PrimaryAttributes->GetHealth() : 0.0f;
}

float ADBCharacterBase::GetMaxHealth() const
{
    return PrimaryAttributes ? PrimaryAttributes->GetMaxHealth() : 1.0f;
}

float ADBCharacterBase::GetHealthPercent() const
{
    float MaxHealth = GetMaxHealth();
    return MaxHealth > 0.0f ? GetHealth() / MaxHealth : 0.0f;
}

float ADBCharacterBase::GetMana() const
{
    return PrimaryAttributes ? PrimaryAttributes->GetMana() : 0.0f;
}

float ADBCharacterBase::GetMaxMana() const
{
    return PrimaryAttributes ? PrimaryAttributes->GetMaxMana() : 1.0f;
}

float ADBCharacterBase::GetManaPercent() const
{
    float MaxMana = GetMaxMana();
    return MaxMana > 0.0f ? GetMana() / MaxMana : 0.0f;
}

int32 ADBCharacterBase::GetCharacterLevel() const
{
    return PrimaryAttributes ? PrimaryAttributes->GetLevel() : 1;
}

// ============================================================================
// ABILITY HELPERS
// ============================================================================

void ADBCharacterBase::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
    if (!HasAuthority() || !AbilitySystemComponent || !AbilityClass) return;

    FGameplayAbilitySpec Spec(AbilityClass, Level, INDEX_NONE, this);
    AbilitySystemComponent->GiveAbility(Spec);
}

void ADBCharacterBase::ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass, int32 Level)
{
    if (!HasAuthority() || !AbilitySystemComponent || !EffectClass) return;

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, Context);
    if (SpecHandle.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

bool ADBCharacterBase::HasGameplayTag(FGameplayTag Tag) const
{
    return AbilitySystemComponent ? AbilitySystemComponent->HasMatchingGameplayTag(Tag) : false;
}

void ADBCharacterBase::AddGameplayTag(FGameplayTag Tag)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(Tag);
    }
}

void ADBCharacterBase::RemoveGameplayTag(FGameplayTag Tag)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
    }
}