#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "Types/EDBJob.h"
#include "Engine/DataTable.h"
#include "DBJobComponent.generated.h"

// ===== Grant record =====
USTRUCT(BlueprintType)
struct FDBGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequiredLevel = 1;
};

USTRUCT(BlueprintType)
struct FDBGrantedAbilityArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDBGrantedAbility> Abilities;
};

// ===== Delegates =====
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJobChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJobLevelChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveGrantsChanged);

// ===== Job component =====
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAWNBLADE_API UDBJobComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDBJobComponent();

	// Current job + level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job", ReplicatedUsing=OnRep_CurrentJob)
	EDBJob CurrentJob = EDBJob::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job", ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	// Catalog of potential abilities by job
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job|Catalog")
	TMap<EDBJob, FDBGrantedAbilityArray> GrantsByJob;

	// Runtime "active" grants = filtered by level and books
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DB|Job|Runtime")
	TArray<FDBGrantedAbility> ActiveGrants;

	// Books learned cross-job (Cure, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DB|Job|Runtime")
	TArray<TSubclassOf<UGameplayAbility>> LearnedAbilities;

	// Optional: hook up your DataTable with FDBAbilityData rows
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job|Catalog")
	UDataTable* AbilityDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job|Catalog")
	bool bAutoBuildCatalogFromTable = false;

	// ===== API =====
	UFUNCTION(BlueprintCallable, Category="DB|Job") EDBJob GetCurrentJob() const { return CurrentJob; }
	UFUNCTION(BlueprintCallable, Category="DB|Job") int32 GetLevel() const { return Level; }

	// Server authoritative setters
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="DB|Job")
	void ServerSetCurrentJob(EDBJob NewJob);

	// Server RPC to learn a new ability
	UFUNCTION(BlueprintCallable, Server, Reliable, Category="DB|Job")
	void ServerLearnAbility(TSubclassOf<class UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category="DB|Job")
	void ServerSetLevel(int32 NewLevel);

	UFUNCTION(Client, Reliable)
	void ClientRequestGrantsRefresh();

	UFUNCTION(Server, Reliable)
	void ServerRebuildGrants();

	// Runtime calls
	UFUNCTION(BlueprintCallable, Category="DB|Job") void RebuildActiveGrants();
	UFUNCTION(BlueprintCallable, Category="DB|Job|Books") void LearnAbilityBook(TSubclassOf<class UGameplayAbility> Ability);
	UFUNCTION(BlueprintCallable, Category="DB|Job|Books") bool HasLearned(TSubclassOf<class UGameplayAbility> Ability) const;

	// Optional: build job catalogs directly from AbilityDataTable
	UFUNCTION(BlueprintCallable, Category="DB|Job|Catalog") void AutoBuildCatalogFromTable();

	// Events
	UPROPERTY(BlueprintAssignable) FOnJobChanged OnJobChanged;
	UPROPERTY(BlueprintAssignable) FOnJobLevelChanged OnJobLevelChanged;
	UPROPERTY(BlueprintAssignable) FOnActiveGrantsChanged OnActiveGrantsChanged;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// RepNotifies
	UFUNCTION() void OnRep_CurrentJob();
	UFUNCTION() void OnRep_Level();
};
