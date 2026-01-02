// Fill out your copyright notice in the Description page of Project Settings.

#include "DBPlayerCharacter.h"

#include "Player/DBPlayerState.h"
#include "Components/DBJobComponent.h"
#include "Components/DBLoadoutComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

ADBPlayerCharacter::ADBPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character with controller - let camera control rotation
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;
	// bUseControllerRotationRoll = false;

	// Configure character movement

	// Create Camera Boom (pulls in towards character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller
	// CameraBoom->bInheritPitch = true;
	// CameraBoom->bInheritYaw = true;
	// CameraBoom->bInheritRoll = false;

	// Enable camera collision for AAA feel
	// CameraBoom->bDoCollisionTest = true;
	// CameraBoom->bEnableCameraLag = true; // Smooth camera movement
	// CameraBoom->CameraLagSpeed = 10.0f;
	// CameraBoom->bEnableCameraRotationLag = true;
	// CameraBoom->CameraRotationLagSpeed = 10.0f;
	
	// Set socket offset for over-the-shoulder view
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f); // Right and up offset

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->FieldOfView = 90.0f; // Standard FOV

	// Job + Loadout
	Job = CreateDefaultSubobject<UDBJobComponent>(TEXT("Job"));
	Loadout = CreateDefaultSubobject<UDBLoadoutComponent>(TEXT("Loadout"));
}

UAbilitySystemComponent* ADBPlayerCharacter::GetAbilitySystemComponent() const
{
	if (const ADBPlayerState* PS = GetPlayerState<ADBPlayerState>())
	{
		return PS->ASC;
	}
	return nullptr;
}

void ADBPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("=== DBPlayerCharacter BeginPlay ==="));

	// Add the Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController found: %s"), *PC->GetName());
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			UE_LOG(LogTemp, Warning, TEXT("EnhancedInput Subsystem found"));
			
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogTemp, Warning, TEXT("Added DefaultMappingContext: %s"), *DefaultMappingContext->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is NULL! Set it in the Blueprint!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EnhancedInput Subsystem is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL!"));
	}

	// Log camera setup
	UE_LOG(LogTemp, Warning, TEXT("CameraBoom: %s, bUsePawnControlRotation: %d"), 
		CameraBoom ? TEXT("Valid") : TEXT("NULL"),
		CameraBoom ? CameraBoom->bUsePawnControlRotation : 0);
	
	UE_LOG(LogTemp, Warning, TEXT("FollowCamera: %s"), 
		FollowCamera ? TEXT("Valid") : TEXT("NULL"));
}

void ADBPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("=== SetupPlayerInputComponent ==="));

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent found"));

		// Movement
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADBPlayerCharacter::Move);
			UE_LOG(LogTemp, Warning, TEXT("MoveAction bound: %s"), *MoveAction->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MoveAction is NULL!"));
		}

		// Looking - THIS IS CRITICAL FOR CAMERA MOVEMENT
		if (LookAction)
		{
			EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADBPlayerCharacter::Look);
			UE_LOG(LogTemp, Warning, TEXT("LookAction bound: %s"), *LookAction->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LookAction is NULL! Camera will NOT rotate!"));
		}

		// Jumping
		if (JumpAction)
		{
			EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			UE_LOG(LogTemp, Warning, TEXT("JumpAction bound: %s"), *JumpAction->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("JumpAction is NULL!"));
		}

		// Abilities
		if (PrimaryAAction)
		{
			EIC->BindAction(PrimaryAAction, ETriggerEvent::Started, this, &ADBPlayerCharacter::UsePrimaryA);
		}
		if (PrimaryBAction)
		{
			EIC->BindAction(PrimaryBAction, ETriggerEvent::Started, this, &ADBPlayerCharacter::UsePrimaryB);
		}
		if (PrimaryXAction)
		{
			EIC->BindAction(PrimaryXAction, ETriggerEvent::Started, this, &ADBPlayerCharacter::UsePrimaryX);
		}
		if (PrimaryYAction)
		{
			EIC->BindAction(PrimaryYAction, ETriggerEvent::Started, this, &ADBPlayerCharacter::UsePrimaryY);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast to EnhancedInputComponent!"));
	}
}

void ADBPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Dead zone - ignore small inputs (stick drift)
	const float DeadZone = 0.15f;
	if (LookAxisVector.Size() < DeadZone)
	{
		return;
	}

	if (Controller != nullptr)
	{
		LookAxisVector.X *= MouseSensitivity;
		LookAxisVector.Y *= MouseSensitivity;

		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADBPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);  // W/S = forward/back
		AddMovementInput(RightDirection, MovementVector.X);    // A/D = left/right
	}
}

void ADBPlayerCharacter::UsePrimaryA() { TryActivateBySlot(0); }
void ADBPlayerCharacter::UsePrimaryB() { TryActivateBySlot(1); }
void ADBPlayerCharacter::UsePrimaryX() { TryActivateBySlot(2); }
void ADBPlayerCharacter::UsePrimaryY() { TryActivateBySlot(3); }

void ADBPlayerCharacter::TryActivateBySlot(int32 SlotIndex)
{
	if (!Loadout)
	{
		return;
	}

	const UDBCombatSetData* ActiveSet = Loadout->GetActiveSet();
	if (!ActiveSet)
	{
		return;
	}

	if (!ActiveSet->Entries.IsValidIndex(SlotIndex))
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	TSubclassOf<UGameplayAbility> TargetAbility = ActiveSet->Entries[SlotIndex].Ability;
	if (!TargetAbility)
	{
		return;
	}

	for (auto& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == TargetAbility)
		{
			ASC->TryActivateAbility(Spec.Handle);
			return;
		}
	}
}

bool ADBPlayerCharacter::ActivateAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass) return false;

	ADBPlayerState* PS = GetPlayerState<ADBPlayerState>();
	UAbilitySystemComponent* ASC = PS ? PS->ASC : nullptr;
	if (!ASC) return false;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->IsA(AbilityClass))
		{
			return ASC->TryActivateAbility(Spec.Handle);
		}
	}
	return false;
}

void ADBPlayerCharacter::GetKnownAbilities(TArray<TSubclassOf<UGameplayAbility>>& OutAbilities) const
{
	OutAbilities.Reset();

	if (Job)
	{
		for (const FDBGrantedAbility& G : Job->ActiveGrants)
		{
			if (G.Ability) OutAbilities.AddUnique(G.Ability);
		}

		for (TSubclassOf<UGameplayAbility> Learned : Job->LearnedAbilities)
		{
			if (Learned) OutAbilities.AddUnique(Learned);
		}
	}
}

void ADBPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ADBPlayerState* PS = GetPlayerState<ADBPlayerState>())
	{
		PS->InitASCForPawn(this);

		if (HasAuthority())
		{
			if (Loadout && Job)
			{
				Loadout->AutoFillFromJob(Job, 8);
			}
			SyncGrantedAbilities();
		}
	}

	if (Job)
	{
		Job->OnActiveGrantsChanged.AddDynamic(this, &ADBPlayerCharacter::HandleActiveGrantsChanged);
		Job->OnJobChanged.AddDynamic(this, &ADBPlayerCharacter::HandleJobChanged);
		Job->OnJobLevelChanged.AddDynamic(this, &ADBPlayerCharacter::HandleJobLevelChanged);
	}
	if (Loadout)
	{
		Loadout->OnActiveSetChanged.AddDynamic(this, &ADBPlayerCharacter::HandleActiveSetChanged);
	}
}

void ADBPlayerCharacter::HandleActiveGrantsChanged()
{
	if (Loadout && Job)
	{
		Loadout->AutoFillFromJob(Job, 8);
	}
	SyncGrantedAbilities();
}

void ADBPlayerCharacter::HandleActiveSetChanged(int32)
{
	SyncGrantedAbilities();
}

void ADBPlayerCharacter::HandleJobChanged()
{
	SyncGrantedAbilities();
}

void ADBPlayerCharacter::HandleJobLevelChanged()
{
	SyncGrantedAbilities();
}

void ADBPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (ADBPlayerState* PS = GetPlayerState<ADBPlayerState>())
	{
		PS->InitASCForPawn(this);
	}
}

void ADBPlayerCharacter::SyncGrantedAbilities()
{
	if (!HasAuthority()) return;

	ADBPlayerState* PS = GetPlayerState<ADBPlayerState>();
	UAbilitySystemComponent* ASC = PS ? PS->ASC : nullptr;
	if (!ASC) return;

	TSet<TSubclassOf<UGameplayAbility>> Desired;

	if (Job)
	{
		for (const auto& GA : Job->ActiveGrants)
		{
			if (GA.Ability && Job->Level >= GA.RequiredLevel)
			{
				Desired.Add(GA.Ability);
			}
		}
	}

	if (const UDBCombatSetData* Active = Loadout ? Loadout->GetActiveSet() : nullptr)
	{
		for (const auto& E : Active->Entries)
		{
			if (E.Ability) Desired.Add(E.Ability);
		}
	}

	for (TSubclassOf<UGameplayAbility> ClassToGrant : Desired)
	{
		if (!ASC_HasAbilityClass(ASC, ClassToGrant))
		{
			const FGameplayAbilitySpec Spec(ClassToGrant, 1, INDEX_NONE);
			const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
			GrantedBySystem.FindOrAdd(ClassToGrant) = Handle;
		}
	}

	TArray<TSubclassOf<UGameplayAbility>> ToErase;
	for (auto& Pair : GrantedBySystem)
	{
		const TSubclassOf<UGameplayAbility> ClassWeGave = Pair.Key;
		const FGameplayAbilitySpecHandle    Handle      = Pair.Value;

		if (!Desired.Contains(ClassWeGave))
		{
			ASC->ClearAbility(Handle);
			ToErase.Add(ClassWeGave);
		}
	}
	for (auto& K : ToErase) { GrantedBySystem.Remove(K); }
}

bool ADBPlayerCharacter::ASC_HasAbilityClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!ASC || !*AbilityClass) return false;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->IsA(AbilityClass))
		{
			return true;
		}
	}
	return false;
}

void ADBPlayerCharacter::ASC_GiveAbilityIfMissing(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (!ASC || !*AbilityClass) return;
	if (!ASC_HasAbilityClass(ASC, AbilityClass))
	{
		ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, Level, INDEX_NONE));
	}
}