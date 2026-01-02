#include "Player/DBPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Attributes/DBAttr_Primary.h"
#include "Attributes/DBAttr_Combat.h"

ADBPlayerState::ADBPlayerState()
{
	ASC = CreateDefaultSubobject<UDBAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttrStats  = CreateDefaultSubobject<UDBAttr_Primary>(TEXT("AttrStats"));
	AttrCombat = CreateDefaultSubobject<UDBAttr_Combat>(TEXT("AttrCombat"));
}

void ADBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ApplyInitStats();
		GrantStartupAbilities();
	}
}

void ADBPlayerState::InitASCForPawn(APawn* NewPawn)
{
	// Owner = this PlayerState, Avatar = Pawn (your Character)
	ASC->InitAbilityActorInfo(this, NewPawn);
}

void ADBPlayerState::ApplyInitStats()
{
	if (!GE_InitStats) return;

	FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle     Sh = ASC->MakeOutgoingSpec(GE_InitStats, 1.f, Ctx);
	if (Sh.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*Sh.Data.Get());
	}
}

void ADBPlayerState::GrantStartupAbilities()
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, /*Level*/1, /*InputID*/INDEX_NONE);
			ASC->GiveAbility(Spec);
		}
	}
}
