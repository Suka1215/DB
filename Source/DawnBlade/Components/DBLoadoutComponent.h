// DBLoadoutComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/ActorComponent.h"
#include "DBLoadoutComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveSetChanged, int32, NewIndex);

USTRUCT(BlueprintType)
struct FDBCombatEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TSubclassOf<class UGameplayAbility> Ability;
	UPROPERTY(EditAnywhere) FText Label;
	UPROPERTY(EditAnywhere) UTexture2D* Icon = nullptr;
	UPROPERTY(EditAnywhere) FName PrimaryButton; // "A","B","X","Y"
};

UCLASS(BlueprintType)
class UDBCombatSetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) FText DisplayName;
	UPROPERTY(EditAnywhere) TArray<FDBCombatEntry> Entries;

	/** Utility: ensures at least N entries exist */
	void EnsureNum(int32 Count)
	{
		if (Entries.Num() < Count) Entries.SetNum(Count);
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBLoadoutComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<UDBCombatSetData*> Sets;

	UFUNCTION(BlueprintCallable) void CycleSet(int32 Delta);
	UFUNCTION(BlueprintCallable) const UDBCombatSetData* GetActiveSet() const { return Sets.IsValidIndex(ActiveSetIndex)? Sets[ActiveSetIndex] : nullptr; }
	UFUNCTION(BlueprintCallable) int32 GetActiveSetIndex() const { return ActiveSetIndex; }

	/** Event: broadcast when active set changes (rep) */
	UPROPERTY(BlueprintAssignable, Category = "Loadout")
	FOnActiveSetChanged OnActiveSetChanged;

	/** Auto-seed empty slots from a job’s MainGrants (non-destructive). Server only. */
	void AutoFillFromJob(class UDBJobComponent* Job, int32 MaxSlots = 8);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(Server, Reliable) void ServerSetActiveSet(int32 NewIndex);

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSetIndex) int32 ActiveSetIndex = 0;
	UFUNCTION() void OnRep_ActiveSetIndex() { OnActiveSetChanged.Broadcast(ActiveSetIndex); }
};
