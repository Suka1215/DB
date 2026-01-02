#include "BP_TestDummy.h"
#include "AbilitySystemComponent.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"
#include "Attributes/DBAttr_Resistance.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"

ABP_TestDummy::ABP_TestDummy()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Create Attribute Sets
    PrimaryAttributes = CreateDefaultSubobject<UDBAttr_Primary>(TEXT("PrimaryAttributes"));
    CombatAttributes = CreateDefaultSubobject<UDBAttr_Combat>(TEXT("CombatAttributes"));
    ResistanceAttributes = CreateDefaultSubobject<UDBAttr_Resistance>(TEXT("ResistanceAttributes"));

    // Health bar widget
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(200.0f, 30.0f));

    // Disable movement - dummy stands still
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // Collision setup
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    GetMesh()->SetCollisionProfileName(TEXT("Pawn"));

    // Actor tag for targeting
    Tags.Add(FName("Enemy"));

    // Gameplay tag
    EnemyTag = FGameplayTag::RequestGameplayTag(FName("Character.Enemy"));
}

void ABP_TestDummy::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        // Initialize ASC
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        
        // Set initial attribute values
        if (PrimaryAttributes)
        {
            // High health for testing
            PrimaryAttributes->SetHealth(5000.0f);
            PrimaryAttributes->SetMaxHealth(5000.0f);
            PrimaryAttributes->SetMana(1000.0f);
            PrimaryAttributes->SetMaxMana(1000.0f);
            
            // Basic stats
            PrimaryAttributes->SetLevel(10.0f);
            PrimaryAttributes->SetStrength(20.0f);
            PrimaryAttributes->SetVitality(30.0f);
            PrimaryAttributes->SetDexterity(15.0f);
            PrimaryAttributes->SetIntelligence(10.0f);
            PrimaryAttributes->SetMind(10.0f);
            
            UE_LOG(LogTemp, Warning, TEXT("🎯 Dummy attributes initialized: HP %.0f / %.0f"), 
                PrimaryAttributes->GetHealth(), PrimaryAttributes->GetMaxHealth());
        }

        if (CombatAttributes)
        {
            // Moderate defense for testing damage
            CombatAttributes->SetDefense(50.0f);
            CombatAttributes->SetEvasion(5.0f);
            CombatAttributes->SetAttack(10.0f);
        }

        // Add enemy tag
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(EnemyTag);
        AbilitySystemComponent->AddLooseGameplayTags(TagContainer);

        // Bind to health changes
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            PrimaryAttributes->GetHealthAttribute()).AddUObject(this, &ABP_TestDummy::OnHealthChanged);
    }

    // Create dynamic material for hit flash
    if (GetMesh() && GetMesh()->GetNumMaterials() > 0)
    {
        UMaterialInterface* Material = GetMesh()->GetMaterial(0);
        if (Material)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            GetMesh()->SetMaterial(0, DynamicMaterial);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("🎯 Test Dummy spawned: %s"), *GetName());
}

void ABP_TestDummy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

UAbilitySystemComponent* ABP_TestDummy::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABP_TestDummy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float Damage = Data.OldValue - Data.NewValue;
    
    if (Damage > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("💥 %s took %.1f damage! Health: %.1f / %.1f"), 
            *GetName(), 
            Damage, 
            Data.NewValue, 
            PrimaryAttributes->GetMaxHealth());

        FlashOnHit();

        // Check for death
        if (Data.NewValue <= 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("💀 %s defeated!"), *GetName());
            // TODO: Death logic
        }
    }
}

void ABP_TestDummy::FlashOnHit()
{
    if (DynamicMaterial)
    {
        // Flash white
        DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::White);
        
        // Reset after 0.1s
        GetWorld()->GetTimerManager().SetTimer(FlashTimer, [this]()
        {
            if (DynamicMaterial)
            {
                DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
            }
        }, 0.1f, false);
    }
}

float ABP_TestDummy::GetHealthPercent() const
{
    if (PrimaryAttributes && PrimaryAttributes->GetMaxHealth() > 0.0f)
    {
        return PrimaryAttributes->GetHealth() / PrimaryAttributes->GetMaxHealth();
    }
    return 1.0f;
}