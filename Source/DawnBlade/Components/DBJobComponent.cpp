#include "Components/DBJobComponent.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "DBEquipmentComponent.h"
#include "DBWeaponComponent.h"
#include "GameFramework/Character.h"
#include "Types/DBWeaponAnimationSet.h"
#include "Types/FDBAbilityData.h" // your struct with ActionType, etc.
#include "Utilities/FDBAbilityDataHelpers.h"

UDBJobComponent::UDBJobComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDBJobComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("=== DBJobComponent BeginPlay ==="));
	UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Owner Role: %d"), GetOwnerRole());
	UE_LOG(LogTemp, Warning, TEXT("Current Job: %d"), (int32)CurrentJob);
	UE_LOG(LogTemp, Warning, TEXT("Level: %d"), Level);
	UE_LOG(LogTemp, Warning, TEXT("GrantsByJob entries: %d"), GrantsByJob.Num());

	// Log what's in the catalog
	for (const auto& Pair : GrantsByJob)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Job %d has %d abilities in catalog"), 
			(int32)Pair.Key, Pair.Value.Abilities.Num());
	}

	if (bAutoBuildCatalogFromTable && GetOwnerRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Auto-building catalog from table..."));
		AutoBuildCatalogFromTable();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Calling RebuildActiveGrants..."));
	RebuildActiveGrants();
}

void UDBJobComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDBJobComponent, CurrentJob);
	DOREPLIFETIME(UDBJobComponent, SubJob);
	DOREPLIFETIME(UDBJobComponent, Level);
}

void UDBJobComponent::OnRep_CurrentJob()
{
	UE_LOG(LogTemp, Log, TEXT("[JobComponent] Job changed to: %d"), (int32)CurrentJob);

	// Get ASC from owner
	UAbilitySystemComponent* ASC = nullptr;
	if (AActor* Owner = GetOwner())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
		{
			ASC = ASI->GetAbilitySystemComponent();
		}
	}

	if (ASC)
	{
		// Remove all weapon tags
		FGameplayTagContainer WeaponTagsToRemove;
		WeaponTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.Staff")));
		WeaponTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.GreatSword")));
		WeaponTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.SwordShield")));
		ASC->RemoveLooseGameplayTags(WeaponTagsToRemove);

		// Remove all job tags
		FGameplayTagContainer JobTagsToRemove;
		JobTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Job.LightMage")));
		JobTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Job.Warrior")));
		JobTagsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("Job.Paladin")));
		ASC->RemoveLooseGameplayTags(JobTagsToRemove);

		// Apply new weapon and job tags based on current job
		FGameplayTag NewWeaponTag;
		FGameplayTag NewJobTag;
		
		switch (CurrentJob)
		{
			case EDBJob::LightMage:
				NewWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.Staff"));
				NewJobTag = FGameplayTag::RequestGameplayTag(FName("Job.LightMage"));
				UE_LOG(LogTemp, Log, TEXT("[JobComponent] Applying Light Mage tags"));
				break;
				
			case EDBJob::Warrior:
				NewWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.GreatSword"));
				NewJobTag = FGameplayTag::RequestGameplayTag(FName("Job.Warrior"));
				UE_LOG(LogTemp, Log, TEXT("[JobComponent] Applying Warrior tags"));
				break;
				
			case EDBJob::Paladin:
				NewWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.SwordShield"));
				NewJobTag = FGameplayTag::RequestGameplayTag(FName("Job.Paladin"));
				UE_LOG(LogTemp, Log, TEXT("[JobComponent] Applying Paladin tags"));
				break;
				
			default:
				UE_LOG(LogTemp, Warning, TEXT("[JobComponent] No weapon tag for job: %d"), (int32)CurrentJob);
				break;
		}
		
		if (NewWeaponTag.IsValid())
		{
			ASC->AddLooseGameplayTag(NewWeaponTag);
			UE_LOG(LogTemp, Log, TEXT("[JobComponent] Added weapon tag: %s"), *NewWeaponTag.ToString());
		}
		
		if (NewJobTag.IsValid())
		{
			ASC->AddLooseGameplayTag(NewJobTag);
			UE_LOG(LogTemp, Log, TEXT("[JobComponent] Added job tag: %s"), *NewJobTag.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[JobComponent] Could not find ASC on owner"));
	}

	// Broadcast and rebuild
	OnJobChanged.Broadcast();
	RebuildActiveGrants();
}

void UDBJobComponent::EquipStartingGear()
{
    UE_LOG(LogTemp, Warning, TEXT("=== EquipStartingGear ==="));
    UE_LOG(LogTemp, Warning, TEXT("Current Job: %d"), (int32)CurrentJob);

    // Get equipment component
    UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>();
    if (!EquipComp)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Equipment Component found"));
        return;
    }

    // Get starting gear for current job
    if (!JobStartingGear.Contains(CurrentJob))
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No starting gear configured for job: %d"), (int32)CurrentJob);
        return;
    }

    const FJobStartingGear& StartingGear = JobStartingGear[CurrentJob];

    // Helper lambda to equip an item if it exists
    auto TryEquipItem = [&](const TSoftObjectPtr<UDBEquipmentItemData>& ItemPtr, const TCHAR* SlotName)
    {
        if (!ItemPtr.IsNull())
        {
            UDBEquipmentItemData* Item = ItemPtr.LoadSynchronous();
            if (Item)
            {
                UE_LOG(LogTemp, Log, TEXT("  ✅ Equipping %s: %s"), SlotName, *Item->ItemName.ToString());
                EquipComp->EquipItem(Item);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("  ⚠️ Failed to load %s item"), SlotName);
            }
        }
    };

    // Equip all slots
    UE_LOG(LogTemp, Warning, TEXT("📦 Equipping weapons..."));
    TryEquipItem(StartingGear.MainHandItem, TEXT("MainHand"));
    TryEquipItem(StartingGear.OffHandItem, TEXT("OffHand"));
    TryEquipItem(StartingGear.RangedItem, TEXT("Ranged"));
    TryEquipItem(StartingGear.AmmoItem, TEXT("Ammo"));

    UE_LOG(LogTemp, Warning, TEXT("🛡️ Equipping armor..."));
    TryEquipItem(StartingGear.HeadItem, TEXT("Head"));
    TryEquipItem(StartingGear.BodyItem, TEXT("Body"));
    TryEquipItem(StartingGear.HandsItem, TEXT("Hands"));
    TryEquipItem(StartingGear.LegsItem, TEXT("Legs"));
    TryEquipItem(StartingGear.FeetItem, TEXT("Feet"));

    UE_LOG(LogTemp, Warning, TEXT("💎 Equipping accessories..."));
    TryEquipItem(StartingGear.BackItem, TEXT("Back"));
    TryEquipItem(StartingGear.WaistItem, TEXT("Waist"));
    TryEquipItem(StartingGear.NeckItem, TEXT("Neck"));
    TryEquipItem(StartingGear.Ring1Item, TEXT("Ring1"));
    TryEquipItem(StartingGear.Ring2Item, TEXT("Ring2"));
    TryEquipItem(StartingGear.Earring1Item, TEXT("Earring1"));
    TryEquipItem(StartingGear.Earring2Item, TEXT("Earring2"));

    UE_LOG(LogTemp, Warning, TEXT("✅ Starting gear equipped"));
}

void UDBJobComponent::OnRep_Level()
{
	OnJobLevelChanged.Broadcast();
	RebuildActiveGrants();
}

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
		OnRep_CurrentJob(); // Call directly on server to apply tags
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

void UDBJobComponent::SetJob(EDBJob NewJob)
{
    if (CurrentJob == NewJob) return;

    // SAFETY CHECK 1: Cannot switch jobs while weapon is drawn
    UDBWeaponComponent* WeaponComp = GetOwner()->FindComponentByClass<UDBWeaponComponent>();
    if (WeaponComp)
    {
        EWeaponState CurrentWeaponState = WeaponComp->GetWeaponState();
        
        if (CurrentWeaponState == EWeaponState::Drawn || 
            CurrentWeaponState == EWeaponState::Engaging || 
            CurrentWeaponState == EWeaponState::Disengaging)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Cannot switch jobs while weapon is drawn! Sheath weapon first."));
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ Job switch allowed - proceeding..."));

    EDBJob OldJob = CurrentJob;
    
    // Save current job's gear before switching
    if (OldJob != EDBJob::None)
    {
        SaveCurrentGear();
    }
    
    CurrentJob = NewJob;
    Level = 1;

    UE_LOG(LogTemp, Warning, TEXT("🔄 Job changed: %d → %d"), (int32)OldJob, (int32)NewJob);

    // Unequip all old gear
    if (UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>())
    {
        // Weapons
        EquipComp->UnequipSlot(EEquipmentSlot::MainHand);
        EquipComp->UnequipSlot(EEquipmentSlot::OffHand);
        EquipComp->UnequipSlot(EEquipmentSlot::Ranged);
        EquipComp->UnequipSlot(EEquipmentSlot::Ammo);
        
        // Armor
        EquipComp->UnequipSlot(EEquipmentSlot::Head);
        EquipComp->UnequipSlot(EEquipmentSlot::Body);
        EquipComp->UnequipSlot(EEquipmentSlot::Hands);
        EquipComp->UnequipSlot(EEquipmentSlot::Legs);
        EquipComp->UnequipSlot(EEquipmentSlot::Feet);
        
        // Accessories
        EquipComp->UnequipSlot(EEquipmentSlot::Back);
        EquipComp->UnequipSlot(EEquipmentSlot::Waist);
        EquipComp->UnequipSlot(EEquipmentSlot::Neck);
        EquipComp->UnequipSlot(EEquipmentSlot::Ring1);
        EquipComp->UnequipSlot(EEquipmentSlot::Ring2);
        EquipComp->UnequipSlot(EEquipmentSlot::Earring1);
        EquipComp->UnequipSlot(EEquipmentSlot::Earring2);
    }

    // Equip new job's gear
    if (NewJob != EDBJob::None)
    {
        // Check if we have saved gear for this job
        if (SavedJobGear.Contains(NewJob))
        {
            UE_LOG(LogTemp, Warning, TEXT("♻️ Restoring previously equipped gear"));
            RestoreSavedGear();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("🆕 First time as this job - equipping starter gear"));
            EquipStartingGear();
        }
    }
    else
    {
        // Job is None - use the WeaponComp we already found
        // CHANGE: Reuse WeaponComp instead of redeclaring
        if (WeaponComp)  // ← Use existing variable
        {
            WeaponComp->UnequipWeapon();
        }
    }

    OnJobChanged.Broadcast();
    RebuildActiveGrants();
}

void UDBJobComponent::SaveCurrentGear()
{
    UE_LOG(LogTemp, Warning, TEXT("💾 Saving current gear for job: %d"), (int32)CurrentJob);

    UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>();
    if (!EquipComp)
    {
        return;
    }

    // Create or get existing saved gear for this job
    FJobEquippedGear& SavedGear = SavedJobGear.FindOrAdd(CurrentJob);
    SavedGear.EquippedItems.Empty();

    // Save all equipped items
    TArray<EEquipmentSlot> AllSlots = {
        EEquipmentSlot::MainHand, EEquipmentSlot::OffHand, EEquipmentSlot::Ranged, EEquipmentSlot::Ammo,
        EEquipmentSlot::Head, EEquipmentSlot::Body, EEquipmentSlot::Hands, EEquipmentSlot::Legs, EEquipmentSlot::Feet,
        EEquipmentSlot::Back, EEquipmentSlot::Waist, EEquipmentSlot::Neck,
        EEquipmentSlot::Ring1, EEquipmentSlot::Ring2, EEquipmentSlot::Earring1, EEquipmentSlot::Earring2
    };

    for (EEquipmentSlot Slot : AllSlots)
    {
        UDBEquipmentItemData* Item = EquipComp->GetEquippedItem(Slot);
        if (Item)
        {
            SavedGear.EquippedItems.Add(Slot, Item);
            UE_LOG(LogTemp, Log, TEXT("  💾 Saved %s in slot %d"), *Item->ItemName.ToString(), (int32)Slot);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ Saved %d items"), SavedGear.EquippedItems.Num());
}

void UDBJobComponent::RestoreSavedGear()
{
    UE_LOG(LogTemp, Warning, TEXT("♻️ Restoring saved gear for job: %d"), (int32)CurrentJob);

    UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>();
    if (!EquipComp)
    {
        return;
    }

    if (!SavedJobGear.Contains(CurrentJob))
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No saved gear found for this job"));
        return;
    }

    const FJobEquippedGear& SavedGear = SavedJobGear[CurrentJob];

    // Re-equip all saved items
    for (const auto& Pair : SavedGear.EquippedItems)
    {
        EEquipmentSlot Slot = Pair.Key;
        UDBEquipmentItemData* Item = Pair.Value;

        if (Item)
        {
            UE_LOG(LogTemp, Log, TEXT("  ♻️ Restoring %s to slot %d"), *Item->ItemName.ToString(), (int32)Slot);
            EquipComp->EquipItem(Item);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ Restored %d items"), SavedGear.EquippedItems.Num());
}

UDBWeaponAnimationSet* UDBJobComponent::GetDefaultWeaponForJob() const
{
	if (JobDefaultWeapons.Contains(CurrentJob))
	{
		TSoftObjectPtr<UDBWeaponAnimationSet> WeaponPtr = JobDefaultWeapons[CurrentJob];
		if (WeaponPtr.IsValid() || WeaponPtr.LoadSynchronous())
		{
			return WeaponPtr.Get();
		}
	}
    
	UE_LOG(LogTemp, Warning, TEXT("⚠️ No default weapon found for job: %d"), (int32)CurrentJob);
	return nullptr;
}

void UDBJobComponent::SetAnimationBlueprintForJob(EDBJob Job, bool bCombatStance)
{
    UE_LOG(LogTemp, Warning, TEXT("=== SetAnimationBlueprintForJob ==="));
    UE_LOG(LogTemp, Warning, TEXT("Job: %d, Combat Stance: %s"), (int32)Job, bCombatStance ? TEXT("Yes") : TEXT("No"));
    
    // Get the body mesh component
    USkeletalMeshComponent* BodyMesh = GetBodyMeshComponent();
    
    if (!BodyMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not find Body mesh component!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Body Mesh: %s"), *BodyMesh->GetName());
    
    // Determine which ABP to use
    TSubclassOf<UAnimInstance> TargetABP;
    
    if (bCombatStance)
    {
        // Use job-specific combat ABP
        if (JobAnimationBlueprints.Contains(Job))
        {
            TargetABP = JobAnimationBlueprints[Job];
            UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to combat ABP for job: %d"), (int32)Job);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ No combat ABP configured for job: %d"), (int32)Job);
            return;
        }
    }
    else
    {
        // Use generic locomotion ABP
        TargetABP = GenericAnimationBlueprint;
        UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to generic ABP"));
    }
    
    if (!TargetABP)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Target ABP is null!"));
        return;
    }
    
    // Get current ABP for comparison
    UAnimInstance* CurrentAnimInstance = BodyMesh->GetAnimInstance();
    if (CurrentAnimInstance && CurrentAnimInstance->GetClass() == TargetABP)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Already using target ABP, skipping switch"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🔄 Current ABP: %s"), 
        CurrentAnimInstance ? *CurrentAnimInstance->GetClass()->GetName() : TEXT("None"));
    UE_LOG(LogTemp, Warning, TEXT("🔄 Target ABP: %s"), *TargetABP->GetName());
    
    // Switch the animation blueprint
    BodyMesh->SetAnimInstanceClass(TargetABP);
    
    // Verify the switch
    UAnimInstance* NewAnimInstance = BodyMesh->GetAnimInstance();
    if (NewAnimInstance && NewAnimInstance->GetClass() == TargetABP)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Animation Blueprint switched successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Animation Blueprint switch failed!"));
    }
}

USkeletalMeshComponent* UDBJobComponent::GetBodyMeshComponent() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return nullptr;
	}
    
	// Find the Body mesh component (for modular metahuman)
	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	Character->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
    
	// Method 1: Find by name "Body" or "Torso"
	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		FString MeshName = MeshComp->GetName();
		if (MeshName.Contains(TEXT("Body")) || MeshName.Contains(TEXT("Torso")))
		{
			return MeshComp;
		}
	}
    
	// Method 2: Fallback - find mesh with most sockets (likely the main body)
	USkeletalMeshComponent* BestMatch = nullptr;
	int32 MaxSockets = 0;
    
	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		TArray<FName> Sockets = MeshComp->GetAllSocketNames();
		if (Sockets.Num() > MaxSockets)
		{
			MaxSockets = Sockets.Num();
			BestMatch = MeshComp;
		}
	}
    
	return BestMatch;
}

void UDBJobComponent::AutoBuildCatalogFromTable()
{
	if (!AbilityDataTable) return;

	GrantsByJob.Reset();
	UE_LOG(LogTemp, Warning, TEXT("=== AutoBuildCatalogFromTable ==="));

	for (auto& Pair : AbilityDataTable->GetRowMap())
	{
		const FDBAbilityData* Row = reinterpret_cast<const FDBAbilityData*>(Pair.Value);
		if (!Row) continue;

		FString AbilityNameStr = Row->AbilityName.ToString();
		FString ClassName;
		
		// Auto-generate class name from CSV name
		// Special case: All Cure levels (Cure1, Cure2, etc.) use DBGA_Cure
		if (AbilityNameStr.StartsWith(TEXT("Cure")))
		{
			ClassName = TEXT("DBGA_Cure");
		}
		else
		{
			// Default: Prefix "DBGA_" to the ability name
			ClassName = FString::Printf(TEXT("DBGA_%s"), *AbilityNameStr);
		}

		// Try to load the class
		FString ClassPath = FString::Printf(TEXT("/Script/DawnBlade.%s"), *ClassName);
		UClass* GAClass = LoadObject<UClass>(nullptr, *ClassPath);
		
		if (!GAClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("  ⚠ Could not load: %s (for %s)"), 
				*ClassName, *AbilityNameStr);
			continue;
		}

		TSubclassOf<UGameplayAbility> AbilityClass = GAClass;
		TArray<EDBJob> TargetJobs;

		// Determine which jobs get this ability
		if (Row->SchoolTags.IsEmpty())
		{
			// Cross-job ability - give to ALL jobs
			TargetJobs.Add(EDBJob::LightMage);
			TargetJobs.Add(EDBJob::Warrior);
			TargetJobs.Add(EDBJob::Paladin);
			UE_LOG(LogTemp, Log, TEXT("  ✅ [ALL] %s → %s"), 
				*AbilityNameStr, *ClassName);
		}
		else
		{
			// Check each school tag
			TArray<FString> SchoolNames;
			
			if (Row->SchoolTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("School.LightMage"))))
			{
				TargetJobs.AddUnique(EDBJob::LightMage);
				SchoolNames.Add(TEXT("LM"));
			}
			if (Row->SchoolTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("School.Warrior"))))
			{
				TargetJobs.AddUnique(EDBJob::Warrior);
				SchoolNames.Add(TEXT("WAR"));
			}
			if (Row->SchoolTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("School.Paladin"))))
			{
				TargetJobs.AddUnique(EDBJob::Paladin);
				SchoolNames.Add(TEXT("PAL"));
			}

			FString SchoolList = FString::Join(SchoolNames, TEXT("+"));
			UE_LOG(LogTemp, Log, TEXT("  ✅ [%s] %s → %s (Lv%d)"), 
				*SchoolList, *AbilityNameStr, *ClassName, Row->Rank);
		}

		// Add to all target job catalogs
		for (EDBJob Job : TargetJobs)
		{
			if (Job != EDBJob::None)
			{
				FDBGrantedAbilityArray& Arr = GrantsByJob.FindOrAdd(Job);
				FDBGrantedAbility Entry;
				Entry.Ability = AbilityClass;
				Entry.RequiredLevel = Row->Rank;
				Arr.Abilities.Add(Entry);
			}
		}
	}
	
	// Summary
	UE_LOG(LogTemp, Warning, TEXT("=== AutoBuild Complete ==="));
	
	for (const auto& JobPair : GrantsByJob)
	{
		FString JobName = UEnum::GetValueAsString(JobPair.Key);
		UE_LOG(LogTemp, Warning, TEXT("  %s: %d abilities"), *JobName, JobPair.Value.Abilities.Num());
		
		for (const FDBGrantedAbility& Entry : JobPair.Value.Abilities)
		{
			UE_LOG(LogTemp, Log, TEXT("    - %s (Level %d)"), 
				*Entry.Ability->GetName(), Entry.RequiredLevel);
		}
	}
}

bool UDBJobComponent::CanUseWeaponType(UDBWeaponAnimationSet* WeaponData) const
{
	if (!WeaponData) return false;

	// Universal weapons (like unarmed)
	if (WeaponData->bAllJobsCanUse)
	{
		return true;
	}

	// Check if current job is in allowed list
	if (WeaponData->AllowedJobs.Contains(CurrentJob))
	{
		// Also check level requirement
		return Level >= WeaponData->RequiredLevel;
	}

	return false;
}

TArray<FGameplayTag> UDBJobComponent::GetAllowedWeaponCategories() const
{
	TArray<FGameplayTag> AllowedCategories;

	switch (CurrentJob)
	{
	case EDBJob::LightMage:
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Magic.Staff"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Magic.Wand"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Unarmed"));
		break;

	case EDBJob::Warrior:
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Melee.TwoHanded"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Melee.Axe"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Melee.GreatSword"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Unarmed"));
		break;

	case EDBJob::Paladin:
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Melee.SwordAndShield"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Melee.Mace"));
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Unarmed"));
		break;

	default:
		AllowedCategories.Add(FGameplayTag::RequestGameplayTag("Weapon.Unarmed"));
		break;
	}

	return AllowedCategories;
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