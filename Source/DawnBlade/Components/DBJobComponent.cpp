#include "Components/DBJobComponent.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "Types/FDBAbilityData.h" // your struct with ActionType, etc.

UDBJobComponent::UDBJobComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDBJobComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoBuildCatalogFromTable && GetOwnerRole() == ROLE_Authority)
	{
		AutoBuildCatalogFromTable();
	}
	RebuildActiveGrants();
}

void UDBJobComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDBJobComponent, CurrentJob);
	DOREPLIFETIME(UDBJobComponent, Level);
}

void UDBJobComponent::OnRep_CurrentJob() { OnJobChanged.Broadcast(); RebuildActiveGrants(); }
void UDBJobComponent::OnRep_Level() { OnJobLevelChanged.Broadcast(); RebuildActiveGrants(); }

void UDBJobComponent::ServerLearnAbility_Implementation(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!*AbilityClass) return;

	// Add if not already present
	LearnedAbilities.AddUnique(AbilityClass);

	// Recompute the filtered list (based on CurrentJob/Level/etc)
	RebuildActiveGrants();
}

void UDBJobComponent::ServerSetCurrentJob_Implementation(EDBJob NewJob)
{
	if (CurrentJob != NewJob)
	{
		CurrentJob = NewJob;
		OnJobChanged.Broadcast();
		RebuildActiveGrants();
	}
}

void UDBJobComponent::ServerSetLevel_Implementation(int32 NewLevel)
{
	if (Level != NewLevel)
	{
		Level = NewLevel;
		OnJobLevelChanged.Broadcast();
		RebuildActiveGrants();
	}
}

void UDBJobComponent::LearnAbilityBook(TSubclassOf<UGameplayAbility> Ability)
{
	if (!*Ability) return;
	if (!LearnedAbilities.Contains(Ability))
	{
		LearnedAbilities.Add(Ability);
		RebuildActiveGrants();
	}
}

bool UDBJobComponent::HasLearned(TSubclassOf<UGameplayAbility> Ability) const
{
	return LearnedAbilities.Contains(Ability);
}

// Helper: requires book?
static bool RequiresBook(const FDBAbilityData& Row)
{
	switch (Row.ActionType)
	{
	case EDBActionType::JobAbility:    return false;
	case EDBActionType::Spell:
	case EDBActionType::Ninjutsu:
	case EDBActionType::WeaponSkill:   return true;
	default:                           return true;
	}
}

void UDBJobComponent::AutoBuildCatalogFromTable()
{
	if (!AbilityDataTable) return;

	GrantsByJob.Reset();

	for (auto& Pair : AbilityDataTable->GetRowMap())
	{
		const FDBAbilityData* Row = reinterpret_cast<const FDBAbilityData*>(Pair.Value);
		if (!Row) continue;

		// Try to find UClass for AbilityName
		TSubclassOf<UGameplayAbility> GAClass = FindObject<UClass>(ANY_PACKAGE, *Row->AbilityName.ToString());
		if (!*GAClass) continue;

		// Map SchoolTag -> Job
		EDBJob TargetJob = EDBJob::None;
		if (Row->SchoolTag == FGameplayTag::RequestGameplayTag(TEXT("School.Divine")))
		{
			TargetJob = EDBJob::LightMage;
		}
		else if (Row->SchoolTag == FGameplayTag::RequestGameplayTag(TEXT("School.Warrior")))
		{
			TargetJob = EDBJob::Warrior;
		}
		// add more jobs here

		if (TargetJob != EDBJob::None)
		{
			FDBGrantedAbilityArray& Arr = GrantsByJob.FindOrAdd(TargetJob);
			FDBGrantedAbility Entry;
			Entry.Ability = GAClass;
			Entry.RequiredLevel = Row->Rank;
			Arr.Abilities.Add(Entry);
		}
	}
}

void UDBJobComponent::RebuildActiveGrants()
{
	ActiveGrants.Empty();

	// Find the set of grants for this job
	if (const FDBGrantedAbilityArray* Found = GrantsByJob.Find(CurrentJob))
	{
		for (const FDBGrantedAbility& G : Found->Abilities)
		{
			if (Level >= G.RequiredLevel && G.Ability)
			{
				ActiveGrants.Add(G);
			}
		}
	}

	// Add learned cross-job abilities
	for (TSubclassOf<UGameplayAbility> Learned : LearnedAbilities)
	{
		if (Learned)
		{
			ActiveGrants.Add({ Learned, 1 });
		}
	}

	// Notify listeners (Character, UI, etc.)
	OnActiveGrantsChanged.Broadcast();
}

// Server RPC to rebuild grants (authority only)
void UDBJobComponent::ServerRebuildGrants_Implementation()
{
	RebuildActiveGrants();
}

// Client RPC: simply ask the server to rebuild → replication will push ActiveGrants
void UDBJobComponent::ClientRequestGrantsRefresh_Implementation()
{
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		ServerRebuildGrants();
	}
}
