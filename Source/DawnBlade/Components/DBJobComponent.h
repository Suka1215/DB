#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/EDBJob.h"
#include "Types/DBEquipmentItemData.h"
#include "Engine/DataTable.h"
#include "DBJobComponent.generated.h"

class UDBWeaponAnimationSet;
class UDBEquipmentItemData;

USTRUCT(BlueprintType)
struct FJobStartingGear
{
    GENERATED_BODY()

    // Weapons
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    TSoftObjectPtr<UDBEquipmentItemData> MainHandItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    TSoftObjectPtr<UDBEquipmentItemData> OffHandItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    TSoftObjectPtr<UDBEquipmentItemData> RangedItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
    TSoftObjectPtr<UDBEquipmentItemData> AmmoItem;

    // Armor
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
    TSoftObjectPtr<UDBEquipmentItemData> HeadItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
    TSoftObjectPtr<UDBEquipmentItemData> BodyItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
    TSoftObjectPtr<UDBEquipmentItemData> HandsItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
    TSoftObjectPtr<UDBEquipmentItemData> LegsItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
    TSoftObjectPtr<UDBEquipmentItemData> FeetItem;

    // Accessories
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> BackItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> WaistItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> NeckItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> Ring1Item;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> Ring2Item;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> Earring1Item;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Accessories")
    TSoftObjectPtr<UDBEquipmentItemData> Earring2Item;
};

USTRUCT(BlueprintType)
struct FJobEquippedGear
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<EEquipmentSlot, UDBEquipmentItemData*> EquippedItems;
};

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	/** Generic ABP used when disengaged (normal movement) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job|Animation")
	TSubclassOf<UAnimInstance> GenericAnimationBlueprint;

	// Map of job to animation blueprint
	UPROPERTY(EditDefaultsOnly, Category = "Job|Animation")
	TMap<EDBJob, TSubclassOf<UAnimInstance>> JobAnimationBlueprints;
    
	/** Switch animation blueprint based on job and combat state */
	UFUNCTION(BlueprintCallable, Category = "Job|Animation")
	void SetAnimationBlueprintForJob(EDBJob Job, bool bCombatStance);
    
	/** Get the body mesh component (for modular metahuman characters) */
	UFUNCTION(BlueprintCallable, Category = "Job|Animation")
	USkeletalMeshComponent* GetBodyMeshComponent() const;

	UFUNCTION(BlueprintPure, Category = "Job")
	bool CanUseWeaponType(UDBWeaponAnimationSet* WeaponData) const;

	UFUNCTION(BlueprintPure, Category = "Job")
	TArray<FGameplayTag> GetAllowedWeaponCategories() const;

	UFUNCTION(BlueprintPure, Category = "Job")
	UDBWeaponAnimationSet* GetDefaultWeaponForJob() const;

	// ===== API =====
	UFUNCTION(BlueprintCallable, Category="DB|Job") EDBJob GetCurrentJob() const { return CurrentJob; }
	UFUNCTION(BlueprintCallable, Category="DB|Job") int32 GetLevel() const { return Level; }
	UFUNCTION(BlueprintCallable, Category = "Job") void SetJob(EDBJob NewJob);

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

	// Current job
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DB|Job", ReplicatedUsing=OnRep_CurrentJob)
	EDBJob CurrentJob = EDBJob::None;
    
	// Subjob
	UPROPERTY(BlueprintReadOnly, Category = "DB|Job", Replicated)
	EDBJob SubJob = EDBJob::None;

	// RepNotifies
	UFUNCTION() void OnRep_CurrentJob();
	UFUNCTION() void OnRep_Level();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job|Starting Gear")
	TMap<EDBJob, FJobStartingGear> JobStartingGear;

	// Runtime: Remember what's equipped per job (for job switching)
	UPROPERTY()
	TMap<EDBJob, FJobEquippedGear> SavedJobGear;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job|Weapons")
	TMap<EDBJob, TSoftObjectPtr<UDBWeaponAnimationSet>> JobDefaultWeapons;
private:
	void EquipStartingGear();
	void SaveCurrentGear();
	void RestoreSavedGear();
};
