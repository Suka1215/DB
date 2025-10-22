// DBLoadoutComponent.cpp

#include "Components/DBLoadoutComponent.h"

#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Components/DBJobComponent.h"

void UDBLoadoutComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDBLoadoutComponent, ActiveSetIndex);
}

void UDBLoadoutComponent::CycleSet(int32 Delta)
{
	if (Sets.Num()==0) return;
	const int32 NewIndex = (ActiveSetIndex + Delta + Sets.Num()) % Sets.Num();
	if (GetOwner()->HasAuthority())
	{
		ActiveSetIndex = NewIndex;
		OnActiveSetChanged.Broadcast(ActiveSetIndex);
	}
	else
	{
		ServerSetActiveSet(NewIndex);
	}
}

void UDBLoadoutComponent::ServerSetActiveSet_Implementation(int32 NewIndex)
{
	if (Sets.IsValidIndex(NewIndex))
	{
		ActiveSetIndex = NewIndex;
		OnActiveSetChanged.Broadcast(ActiveSetIndex);
	}
}

void UDBLoadoutComponent::AutoFillFromJob(UDBJobComponent* Job, int32 MaxSlots)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!Job) return;

	UDBCombatSetData* Active = const_cast<UDBCombatSetData*>(GetActiveSet());
	if (!Active)
	{
		// safety: if no set, but an asset exists in Sets[0], use it
		if (Sets.IsValidIndex(0)) Active = Sets[0];
		if (!Active) return;
	}

	Active->EnsureNum(MaxSlots);

	// Fill empty slots from MainGrants in order
	int32 GrantIdx = 0;
	for (int32 Slot = 0; Slot < Active->Entries.Num(); ++Slot)
	{
		if (Active->Entries[Slot].Ability) continue;

		// find next usable grant
		while (Job->ActiveGrants.IsValidIndex(GrantIdx))
		{
			const auto& G = Job->ActiveGrants[GrantIdx++];
			if (G.Ability && Job->Level >= G.RequiredLevel)
			{
				Active->Entries[Slot].Ability = G.Ability;
				Active->Entries[Slot].Label   = FText::FromString(G.Ability->GetName());
				break;
			}
		}
		// stop if we exhausted grants
		if (!Job->ActiveGrants.IsValidIndex(GrantIdx)) break;
	}

	// Notify UI
	OnActiveSetChanged.Broadcast(ActiveSetIndex);
}
