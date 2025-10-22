#include "Abilities/LM/DBGA_SacredRegen.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h" // SphereOverlapActors
#include "Engine/EngineTypes.h"         // UEngineTypes::ConvertToObjectType

UDBGA_SacredRegen::UDBGA_SacredRegen()
{
	// Per-ability knobs
	MPCost            = 30.f;
	TPCost            = 0.f;
	ActionLockSeconds = 2.f;    // cast time
	DurationSeconds   = 0.f;    // handled by GE
	CooldownSeconds   = 30.f;
	Range             = 1200.f;

	HealingPerTick       = 100.f;
	RegenerationDuration = 10.f;
	TickInterval         = 1.f;
	AoERadius            = 600.f;

	CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.LightMage.SacredRegen"));
}

void UDBGA_SacredRegen::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!PayCostsAndLock(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* CasterActor = GetAvatarActorFromActorInfo();
	if (!ASC || !CasterActor || !GE_SacredRegen)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Resolve target location (prefer explicit target actor if present)
	FVector TargetLocation = CasterActor->GetActorLocation();
	if (TriggerEventData)
	{
		if (const AActor* TargetConst = TriggerEventData->Target.Get())
		{
			TargetLocation = TargetConst->GetActorLocation();
		}
	}

	// Gather allies
	TArray<AActor*> AlliesInRange = GetAlliesInRadius(TargetLocation);
	if (AlliesInRange.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sacred Regen: No allies in range"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Apply HoT GameplayEffect to allies
	for (AActor* Ally : AlliesInRange)
	{
		if (!Ally) continue;

		if (UAbilitySystemComponent* AllyASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Ally))
		{
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_SacredRegen, GetAbilityLevel(), ASC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Duration")), RegenerationDuration);
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Healing")),  HealingPerTick);
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Period")),   TickInterval);

				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AllyASC);

				UE_LOG(LogTemp, Log, TEXT("Sacred Regen: Applied to %s"), *Ally->GetName());
			}
		}
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (UWorld* World = GetWorld())
	{
		DrawDebugSphere(World, TargetLocation, AoERadius, 16, FColor::Green, false, 3.0f, 0, 1.5f);
	}
#endif

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

TArray<AActor*> UDBGA_SacredRegen::GetAlliesInRadius(const FVector& Location) const
{
	TArray<AActor*> Allies;

	UWorld* World = GetWorld();
	if (!World) return Allies;

	// Build overlap params (Actors only; we’ll filter to "has ASC" below)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjTypes;
	ObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	if (AActor* Self = GetAvatarActorFromActorInfo())
	{
		ActorsToIgnore.Add(Self);
	}

	TArray<AActor*> OutActors;
	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		Location,
		AoERadius,
		ObjTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	if (bHit)
	{
		for (AActor* Candidate : OutActors)
		{
			if (!Candidate) continue;

			// Basic ally heuristic: has an ASC and is not the caster.
			if (UAbilitySystemComponent* AllyASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Candidate))
			{
				Allies.Add(Candidate);
			}
		}
	}

	// Always include the caster themselves (unique)
	if (AActor* Self = GetAvatarActorFromActorInfo())
	{
		Allies.AddUnique(Self);
	}

	return Allies;
}
