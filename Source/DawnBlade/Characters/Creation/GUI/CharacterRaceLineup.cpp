#include "CharacterRaceLineup.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "GroomComponent.h"
#include "Characters/Creation/Types/CharacterCreatorTypes.h"
#include "UObject/UnrealTypePrivate.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "EngineUtils.h" // For TActorIterator

ACharacterRaceLineup::ACharacterRaceLineup()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    LineupCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("LineupCamera"));
    LineupCamera->SetupAttachment(RootComponent);
    LineupCamera->SetFieldOfView(CameraFOV);
}

void ACharacterRaceLineup::BeginPlay()
{
    Super::BeginPlay();

    CharacterSpotlights.Empty();
    TargetSpotlightIntensities.Empty();

    UE_LOG(LogTemp, Warning, TEXT("=== CharacterRaceLineup BeginPlay - CHARACTER FOCUS ==="));

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetViewTarget(this);
    }

    SpawnedCharacters.Empty();
    SpawnedHairGrooms.Empty();

    if (!CreatorDataAsset || CreatorDataAsset->RacePresets.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] CreatorDataAsset missing or empty"), *GetName());
        return;
    }

    // Characters positioned in a dramatic arc formation
    const float ArcRadius = 350.f;
    const float ArcAngle = 60.f; // Total arc span in degrees
    int32 NumPresets = CreatorDataAsset->RacePresets.Num();

    // Pre-size groom array to avoid index issues
    SpawnedHairGrooms.SetNum(NumPresets);

    for (int32 i = 0; i < NumPresets; i++)
    {
        const FRacePreset& Preset = CreatorDataAsset->RacePresets[i];

        // === Straight line formation with fixed positioning ===
        const float Spacing = 150.f; // distance between characters
        float OffsetY = (i - (NumPresets - 1) / 2.0f) * Spacing; 
        FVector CharacterPosition = FVector(0.f, OffsetY, 50.f); // Fixed Z position at 50 units

        // === Body ===
        USkeletalMeshComponent* Body = NewObject<USkeletalMeshComponent>(
            this, *FString::Printf(TEXT("Body_%d"), i));
        Body->RegisterComponent();
        AddInstanceComponent(Body);
        Body->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        if (USkeletalMesh* LoadedBody = Preset.BodyMesh.LoadSynchronous())
        {
            Body->SetSkeletalMesh(LoadedBody);
        }

        Body->SetRelativeLocation(CharacterPosition);

        // Make ALL characters face the same direction toward the camera
        // Camera is positioned at negative X, so all characters should face -X direction
        FRotator CameraFacingRotation = FRotator(0.f, 90.f, 0.f); 
        Body->SetRelativeRotation(CameraFacingRotation);

        // === Modular parts ===
        auto AttachPart = [&](TSoftObjectPtr<USkeletalMesh> Mesh, const FString& PartName)
        {
            if (!Mesh.IsNull())
            {
                if (USkeletalMesh* Loaded = Mesh.LoadSynchronous())
                {
                    USkeletalMeshComponent* Part = NewObject<USkeletalMeshComponent>(
                        this, *FString::Printf(TEXT("%s_%d"), *PartName, i));
                    Part->RegisterComponent();
                    AddInstanceComponent(Part);
                    Part->AttachToComponent(Body, FAttachmentTransformRules::KeepRelativeTransform);
                    Part->SetLeaderPoseComponent(Body);
                    Part->SetSkeletalMesh(Loaded);
                }
            }
        };

        AttachPart(Preset.HeadMesh, TEXT("Head"));
        AttachPart(Preset.HandsMesh, TEXT("Hands"));
        AttachPart(Preset.LegsMesh, TEXT("Legs"));
        AttachPart(Preset.FeetMesh, TEXT("Feet"));

        // === Hair Groom ===
        AttachRaceHairToCharacter(i, Body, Preset);

        // === Animation Setup ===
        if (FallbackAnimBP)
        {
            Body->SetAnimInstanceClass(FallbackAnimBP);
            UE_LOG(LogTemp, Warning, TEXT("Character %d: Set AnimBP - %s"), i, *FallbackAnimBP->GetName());
        }
        else if (!Preset.IdleAnimation.IsNull())
        {
            if (UAnimationAsset* IdleAnim = Preset.IdleAnimation.LoadSynchronous())
            {
                Body->PlayAnimation(IdleAnim, true);
                UE_LOG(LogTemp, Warning, TEXT("Character %d: Playing idle animation - %s"), i, *IdleAnim->GetName());
            }
        }

        // === Lighting with performance optimization ===
        USpotLightComponent* MainSpotlight = NewObject<USpotLightComponent>(
            this, *FString::Printf(TEXT("MainSpotlight_%d"), i));
        MainSpotlight->RegisterComponent();
        AddInstanceComponent(MainSpotlight);
        MainSpotlight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        FVector SpotlightPos = CharacterPosition + FVector(-150.f, 0.f, 250.f);
        MainSpotlight->SetRelativeLocation(SpotlightPos);
        MainSpotlight->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
        MainSpotlight->SetIntensity(0.0f); // Start with all lights off
        MainSpotlight->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f, 1.0f));
        MainSpotlight->SetInnerConeAngle(8.0f); // Tighter inner cone to reduce spillover
        MainSpotlight->SetOuterConeAngle(25.0f); // Much tighter outer cone
        MainSpotlight->SetAttenuationRadius(400.0f); // Reduced range to prevent spillover
        
        // Performance optimization: Reduce shadow quality and disable for non-selected
        MainSpotlight->SetCastShadows(false); // Start with shadows disabled
        // Note: SetShadowResolutionScale not available in UE 5.6, using alternative approach
        MainSpotlight->SetLightFunctionScale(FVector(1.0f, 1.0f, 1.0f)); // Default scale

        USpotLightComponent* RimLight = NewObject<USpotLightComponent>(
            this, *FString::Printf(TEXT("RimLight_%d"), i));
        RimLight->RegisterComponent();
        AddInstanceComponent(RimLight);
        RimLight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        FVector RimLightPos = CharacterPosition + FVector(200.f, 0.f, 150.f);
        RimLight->SetRelativeLocation(RimLightPos);
        RimLight->SetRelativeRotation((CharacterPosition - RimLightPos).Rotation());
        RimLight->SetIntensity(0.0f); // Start with all rim lights off
        RimLight->SetLightColor(FLinearColor(0.4f, 0.6f, 1.0f, 1.0f));
        RimLight->SetInnerConeAngle(5.0f); // Much tighter rim light
        RimLight->SetOuterConeAngle(15.0f); // Tighter outer cone for rim light
        RimLight->SetAttenuationRadius(300.0f); // Reduced range
        RimLight->SetCastShadows(false); // Never cast shadows for rim lights

        CharacterSpotlights.Add(MainSpotlight);
        TargetSpotlightIntensities.Add(0.0f);

        SpawnedCharacters.Add(Body);

        UE_LOG(LogTemp, Warning, TEXT("Character %d: Positioned at %s"), 
            i, *CharacterPosition.ToString());
    }

    // Hide any debug spheres or collision visualizers - check ALL actors in world
    UWorld* World = GetWorld();
    if (World)
    {
        for (TActorIterator<AActor> ActorIterator(World); ActorIterator; ++ActorIterator)
        {
            AActor* Actor = *ActorIterator;
            if (Actor && Actor != this)
            {
                // Check all static mesh components on all actors
                TArray<UStaticMeshComponent*> StaticMeshComponents;
                Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
                
                for (UStaticMeshComponent* StaticMesh : StaticMeshComponents)
                {
                    if (StaticMesh && StaticMesh->GetStaticMesh())
                    {
                        FString MeshName = StaticMesh->GetStaticMesh()->GetName();
                        FString ActorName = Actor->GetName();
                        
                        // Check for sphere/ball in mesh name or actor name
                        if (MeshName.Contains("Sphere") || MeshName.Contains("Ball") ||
                            ActorName.Contains("Sphere") || ActorName.Contains("Ball") ||
                            MeshName.Contains("Debug") || ActorName.Contains("Debug"))
                        {
                            StaticMesh->SetVisibility(false);
                            UE_LOG(LogTemp, Warning, TEXT("Hidden debug object: Actor=%s, Mesh=%s"), 
                                *ActorName, *MeshName);
                        }
                    }
                }
            }
        }
    }

    // Characters are positioned at fixed Z=50, no ground positioning needed
    
    CurrentIndex = 0;
    
    // Initialize the scene
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        ShowRaceAtIndex(CurrentIndex);
        
        // CRITICAL: Force all hair to be visible and disable LOD after initialization
        ForceAllHairVisible();
    });

    UE_LOG(LogTemp, Warning, TEXT("=== CharacterRaceLineup Cinematic Init Complete ==="));
}

void ACharacterRaceLineup::SpawnSmokeEffects()
{
    // Environmental effects removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Environmental effects moved to editor level"));
}

void ACharacterRaceLineup::UpdateEnvironmentalEffects(float DeltaTime)
{
    // Environmental effects removed - handle in editor level
}

void ACharacterRaceLineup::SetCinematicLighting()
{
    // Lighting setup removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Lighting setup moved to editor level"));
}

void ACharacterRaceLineup::SetupCinematicEnvironment()
{
    // Environment setup removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Environment setup moved to editor level"));
}

// === Missing Sci-Fi Functions (Now Deprecated) ===
void ACharacterRaceLineup::SetupSciFiEnvironment()
{
    // Sci-fi environment removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Sci-Fi environment setup moved to editor level"));
}

void ACharacterRaceLineup::SpawnFloatingOrbs()
{
    // Floating orbs removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Floating orbs moved to editor level"));
}

void ACharacterRaceLineup::AnimateSciFiElements(float DeltaTime)
{
    // Sci-fi animation removed - handle in editor level
    UE_LOG(LogTemp, Warning, TEXT("Sci-Fi animations moved to editor level"));
}

void ACharacterRaceLineup::AttachRaceHairToCharacter(
    int32 CharacterIndex, 
    USkeletalMeshComponent* BodyComponent, 
    const FRacePreset& RacePreset)
{
    if (!BodyComponent || !CreatorDataAsset) return;

    // Defensive guard
    if (!SpawnedHairGrooms.IsValidIndex(CharacterIndex))
    {
        SpawnedHairGrooms.SetNum(CharacterIndex + 1);
    }

    CleanupHairForCharacter(CharacterIndex);

    // Load groom
    UGroomAsset* LoadedGroom = RacePreset.RaceHairGroom.LoadSynchronous();
    if (!LoadedGroom)
    {
        UE_LOG(LogTemp, Error, TEXT("Character %d: Groom asset missing"), CharacterIndex);
        return;
    }

    UGroomComponent* GroomComp = NewObject<UGroomComponent>(
        this, *FString::Printf(TEXT("RaceHair_%d"), CharacterIndex));
    GroomComp->RegisterComponent();
    AddInstanceComponent(GroomComp);
    GroomComp->SetGroomAsset(LoadedGroom);

    // Try to load binding
    if (!RacePreset.RaceHairBinding.IsNull())
    {
        if (UGroomBindingAsset* LoadedBinding = RacePreset.RaceHairBinding.LoadSynchronous())
        {
            GroomComp->SetBindingAsset(LoadedBinding);
            UE_LOG(LogTemp, Warning, TEXT("Character %d: Applied RaceHairBinding %s"),
                CharacterIndex, *LoadedBinding->GetName());
        }
    }

    // === PERFORMANCE OPTIMIZATIONS FOR EPIC QUALITY ===
    
    // 1. Disable simulation for non-selected characters (HUGE performance gain)
    bool bIsSelected = (CharacterIndex == CurrentIndex);
    GroomComp->SetEnableSimulation(bIsSelected); // Only simulate selected character
    
    // 2. Set appropriate LOD based on selection and distance
    if (bIsSelected)
    {
        GroomComp->SetForcedLOD(0); // Best quality for selected
    }
    else
    {
        GroomComp->SetForcedLOD(2); // Lower quality for background characters
    }
    
    // 3. Reduce strand count for non-selected characters
    if (!bIsSelected)
    {
        // This requires modifying the groom asset's strand count via console command
        // We'll handle this in the positioning function
    }
    
    // 4. Disable expensive features for background characters
    GroomComp->SetCastShadow(bIsSelected); // Only selected character casts hair shadows
    
    // 5. Set visibility culling aggressively for non-selected
    if (!bIsSelected)
    {
        GroomComp->SetCullDistance(1000.0f); // Cull background hair at medium distance
    }
    else
    {
        GroomComp->SetCullDistance(0.0f); // Never cull selected character
    }

    // === Find Head component (attached as child of Body) ===
    USkeletalMeshComponent* HeadComp = nullptr;
    for (USceneComponent* Child : BodyComponent->GetAttachChildren())
    {
        if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(Child))
        {
            if (SkelComp->GetName().Contains("Head"))
            {
                HeadComp = SkelComp;
                break;
            }
        }
    }

    if (!HeadComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Character %d: No Head mesh found, cannot attach groom!"), CharacterIndex);
        return;
    }

    // === Attach to FACIAL_C_FacialRoot on Head ===
    FName AttachBone = TEXT("FACIAL_C_FacialRoot");
    if (HeadComp->GetBoneIndex(AttachBone) == INDEX_NONE)
    {
        UE_LOG(LogTemp, Error, TEXT("Character %d: Head mesh missing FACIAL_C_FacialRoot, cannot attach groom!"), CharacterIndex);
        return;
    }

    GroomComp->AttachToComponent(
        HeadComp,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        AttachBone);

    SpawnedHairGrooms[CharacterIndex] = GroomComp;

    UE_LOG(LogTemp, Warning, TEXT("Character %d: SUCCESS groom attach -> %s (Selected: %s, Simulation: %s)"), 
        CharacterIndex, *AttachBone.ToString(), bIsSelected ? TEXT("YES") : TEXT("NO"), 
        bIsSelected ? TEXT("ON") : TEXT("OFF"));
}

void ACharacterRaceLineup::CleanupHairForCharacter(int32 CharacterIndex)
{
    if (SpawnedHairGrooms.IsValidIndex(CharacterIndex) && SpawnedHairGrooms[CharacterIndex])
    {
        SpawnedHairGrooms[CharacterIndex]->DestroyComponent();
        SpawnedHairGrooms[CharacterIndex] = nullptr;
    }
}

void ACharacterRaceLineup::ShowRaceAtIndex(int32 Index)
{
    UE_LOG(LogTemp, Warning, TEXT("=== CINEMATIC ShowRaceAtIndex START: Requested=%d, Current=%d ==="), Index, CurrentIndex);
    
    if (SpawnedCharacters.Num() == 0) 
    {
        UE_LOG(LogTemp, Error, TEXT("ShowRaceAtIndex: No spawned characters to show"));
        return;
    }

    int32 PreviousIndex = CurrentIndex;
    CurrentIndex = FMath::Clamp(Index, 0, SpawnedCharacters.Num() - 1);
    
    // === HAIR PERFORMANCE OPTIMIZATION ON SELECTION CHANGE ===
    for (int32 i = 0; i < SpawnedHairGrooms.Num(); i++)
    {
        if (SpawnedHairGrooms[i])
        {
            bool bIsSelected = (i == CurrentIndex);
            
            // Only disable simulation for non-selected (keep hair visible)
            SpawnedHairGrooms[i]->SetEnableSimulation(bIsSelected);
            
            // Keep all hair at good quality (LOD 1 for non-selected, LOD 0 for selected)
            SpawnedHairGrooms[i]->SetForcedLOD(bIsSelected ? 0 : 1);
            
            // Keep shadows for all characters
            SpawnedHairGrooms[i]->SetCastShadow(true);
            
            // Don't cull any hair
            SpawnedHairGrooms[i]->SetCullDistance(0.0f);
            
            UE_LOG(LogTemp, Warning, TEXT("Hair %d: Updated for selection (Selected: %s, LOD: %d)"), 
                i, bIsSelected ? TEXT("YES") : TEXT("NO"), bIsSelected ? 0 : 1);
        }
    }
    
    // Character animation states
    for (int32 i = 0; i < SpawnedCharacters.Num(); i++)
    {
        if (!SpawnedCharacters[i]) continue;

        if (UAnimInstance* AnimInst = SpawnedCharacters[i]->GetAnimInstance())
        {
            UClass* AnimClass = AnimInst->GetClass();
            FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(AnimClass, TEXT("bIsSelected"));
        
            if (BoolProp)
            {
                bool CurrentSelected = BoolProp->GetPropertyValue_InContainer(AnimInst);
                bool ShouldBeSelected = (i == CurrentIndex);
            
                if (!CurrentSelected && ShouldBeSelected)
                {
                    BoolProp->SetPropertyValue_InContainer(AnimInst, true);
                    UE_LOG(LogTemp, Warning, TEXT("Character %d: BECAME SELECTED"), i);
                }
                else if (CurrentSelected && !ShouldBeSelected)
                {
                    BoolProp->SetPropertyValue_InContainer(AnimInst, false);
                    UE_LOG(LogTemp, Warning, TEXT("Character %d: BECAME UNSELECTED"), i);
                }
            }
        }
    }

    // Set spotlight intensities and shadows - ONLY light up the selected character
    for (int32 j = 0; j < CharacterSpotlights.Num(); j++)
    {
        if (TargetSpotlightIntensities.IsValidIndex(j) && CharacterSpotlights[j])
        {
            float OldTarget = TargetSpotlightIntensities[j];
            // Only the selected character gets spotlight, others get 0
            float NewTarget = (j == CurrentIndex) ? CharacterSpotlightIntensity : 0.0f;
            TargetSpotlightIntensities[j] = NewTarget;
            
            // Performance: Only enable shadows for the selected character
            bool bShouldCastShadows = (j == CurrentIndex);
            CharacterSpotlights[j]->SetCastShadows(bShouldCastShadows);
            
            UE_LOG(LogTemp, Warning, TEXT("Cinematic Spotlight %d: Target %f -> %f, Shadows: %s (Selected: %s)"), 
                j, OldTarget, NewTarget, bShouldCastShadows ? TEXT("ON") : TEXT("OFF"), (j == CurrentIndex) ? TEXT("YES") : TEXT("NO"));
        }
    }

    // Also handle rim lights - find and control them
    TArray<UActorComponent*> AllComponents = GetComponents().Array();
    for (UActorComponent* Component : AllComponents)
    {
        if (USpotLightComponent* RimLight = Cast<USpotLightComponent>(Component))
        {
            if (RimLight->GetName().Contains("RimLight"))
            {
                // Extract character index from rim light name
                FString LightName = RimLight->GetName();
                int32 UnderscoreIndex;
                if (LightName.FindLastChar('_', UnderscoreIndex))
                {
                    FString IndexStr = LightName.Mid(UnderscoreIndex + 1);
                    int32 LightCharacterIndex = FCString::Atoi(*IndexStr);
                    
                    // Only light up rim light for selected character
                    float TargetRimIntensity = (LightCharacterIndex == CurrentIndex) ? RimLightIntensity : 0.0f;
                    RimLight->SetIntensity(TargetRimIntensity);
                    
                    UE_LOG(LogTemp, Warning, TEXT("RimLight %d: Set intensity to %f"), 
                        LightCharacterIndex, TargetRimIntensity);
                }
            }
        }
    }

    UpdateCameraTarget();
    
    UE_LOG(LogTemp, Warning, TEXT("=== CINEMATIC ShowRaceAtIndex COMPLETE ==="));
}

void ACharacterRaceLineup::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Montage completed: %s, Interrupted: %s"), 
        Montage ? *Montage->GetName() : TEXT("NULL"), 
        bInterrupted ? TEXT("YES") : TEXT("NO"));
}

void ACharacterRaceLineup::TestMontageOnCurrentCharacter()
{
    if (!SpawnedCharacters.IsValidIndex(CurrentIndex)) return;
    
    UAnimInstance* AnimInst = SpawnedCharacters[CurrentIndex]->GetAnimInstance();
    if (!AnimInst) 
    {
        UE_LOG(LogTemp, Error, TEXT("No AnimInstance for manual test"));
        return;
    }
    
    if (CreatorDataAsset && CreatorDataAsset->RacePresets.IsValidIndex(CurrentIndex))
    {
        const FRacePreset& Preset = CreatorDataAsset->RacePresets[CurrentIndex];
        if (!Preset.SelectAnimation.IsNull())
        {
            if (UAnimMontage* Montage = Preset.SelectAnimation.LoadSynchronous())
            {
                UE_LOG(LogTemp, Warning, TEXT("=== MANUAL MONTAGE TEST ==="));
                UE_LOG(LogTemp, Warning, TEXT("AnimBP Class: %s"), AnimInst->GetClass() ? *AnimInst->GetClass()->GetName() : TEXT("NULL"));
                UE_LOG(LogTemp, Warning, TEXT("Montage: %s"), *Montage->GetName());
                
                AnimInst->StopAllMontages(0.0f);
                float Duration = AnimInst->Montage_Play(Montage, 1.0f);
                UE_LOG(LogTemp, Warning, TEXT("Manual test result - Duration: %f"), Duration);
            }
        }
    }
}

void ACharacterRaceLineup::UpdateCameraTarget()
{
    if (!SpawnedCharacters.IsValidIndex(CurrentIndex)) 
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateCameraTarget: Invalid CurrentIndex %d"), CurrentIndex);
        return;
    }

    FVector CharLoc = SpawnedCharacters[CurrentIndex]->GetComponentLocation();
    
    // Cinematic camera positioning - more dynamic and closer
    CameraTargetLocation = CharLoc + FVector(-CameraDistance, 0.f, CameraHeight);
    FVector LookAtPoint = CharLoc + FVector(0.f, 0.f, 120.f); // Look at chest level
    CameraTargetRotation = (LookAtPoint - CameraTargetLocation).Rotation();
    
    // Add slight camera shake effect for cinematic feel
    float TimeSeconds = GetWorld()->GetTimeSeconds();
    float ShakeX = FMath::Sin(TimeSeconds * 0.5f) * 2.f;
    float ShakeZ = FMath::Cos(TimeSeconds * 0.3f) * 1.5f;
    CameraTargetLocation += FVector(ShakeX, 0.f, ShakeZ);
    
    UE_LOG(LogTemp, Warning, TEXT("Cinematic Camera targeting character %d at %s"), 
        CurrentIndex, *CharLoc.ToString());
}

void ACharacterRaceLineup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Smooth cinematic camera movement
    if (LineupCamera)
    {
        FVector NewLoc = FMath::VInterpTo(LineupCamera->GetComponentLocation(),
                                          CameraTargetLocation,
                                          DeltaTime, CameraLerpSpeed);
        FRotator NewRot = FMath::RInterpTo(LineupCamera->GetComponentRotation(),
                                           CameraTargetRotation,
                                           DeltaTime, CameraLerpSpeed);

        LineupCamera->SetWorldLocationAndRotation(NewLoc, NewRot);
    }

    // Smooth lighting transitions for dramatic effect
    for (int32 i = 0; i < CharacterSpotlights.Num(); i++)
    {
        if (CharacterSpotlights.IsValidIndex(i) && CharacterSpotlights[i] && 
            TargetSpotlightIntensities.IsValidIndex(i))
        {
            float CurrentIntensity = CharacterSpotlights[i]->Intensity;
            float TargetIntensity = TargetSpotlightIntensities[i];
            
            float NewIntensity = FMath::FInterpTo(CurrentIntensity, TargetIntensity, 
                                                  DeltaTime, LightTransitionSpeed);
            CharacterSpotlights[i]->SetIntensity(NewIntensity);
        }
    }

    // Update environmental effects - removed, handled in editor level
    // UpdateEnvironmentalEffects(DeltaTime); // REMOVED
}

void ACharacterRaceLineup::NextRace()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (CurrentTime - LastSelectionTime < SelectionCooldown)
    {
        UE_LOG(LogTemp, Error, TEXT("NextRace BLOCKED - Too soon! Cooldown: %f"), SelectionCooldown);
        return;
    }
    
    LastSelectionTime = CurrentTime;
    int32 NewIndex = CurrentIndex + 1;
    ShowRaceAtIndex(NewIndex);
}

void ACharacterRaceLineup::PrevRace()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (CurrentTime - LastSelectionTime < SelectionCooldown)
    {
        UE_LOG(LogTemp, Error, TEXT("PrevRace BLOCKED - Too soon! Cooldown: %f"), SelectionCooldown);
        return;
    }
    
    LastSelectionTime = CurrentTime;
    int32 NewIndex = CurrentIndex - 1;
    ShowRaceAtIndex(NewIndex);
}

FRacePreset ACharacterRaceLineup::GetCurrentRace() const
{
    return (CreatorDataAsset && CreatorDataAsset->RacePresets.IsValidIndex(CurrentIndex))
        ? CreatorDataAsset->RacePresets[CurrentIndex]
        : FRacePreset();
}

void ACharacterRaceLineup::RefreshAllHair()
{
    if (!CreatorDataAsset) return;

    for (int32 i = 0; i < SpawnedCharacters.Num(); i++)
    {
        if (SpawnedCharacters[i] && CreatorDataAsset->RacePresets.IsValidIndex(i))
        {
            AttachRaceHairToCharacter(i, SpawnedCharacters[i], CreatorDataAsset->RacePresets[i]);
        }
    }
}

void ACharacterRaceLineup::SetHairVisibility(bool bVisible)
{
    for (UGroomComponent* GroomComp : SpawnedHairGrooms)
    {
        if (GroomComp)
        {
            GroomComp->SetVisibility(bVisible);
        }
    }
}

void ACharacterRaceLineup::ForceAllHairVisible()
{
    for (UGroomComponent* GroomComp : SpawnedHairGrooms)
    {
        if (GroomComp)
        {
            // Only set basic visibility without forcing render state changes
            GroomComp->SetVisibility(true, false); // Don't propagate to children
            GroomComp->SetCullDistance(0.0f); // Never cull
            GroomComp->SetHiddenInGame(false); // Ensure not hidden
            
            // DON'T force render state dirty as it can reset materials
            // GroomComp->MarkRenderStateDirty(); // REMOVED
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Forced all hair visible - Total grooms: %d"), SpawnedHairGrooms.Num());
}

void ACharacterRaceLineup::DisableHairLODForAllCharacters()
{
    for (UGroomComponent* GroomComp : SpawnedHairGrooms)
    {
        if (GroomComp)
        {
            // Set appropriate LOD based on scalability instead of forcing LOD 0
            int32 TargetLOD = 0;
            
            // Check current scalability settings and adjust LOD accordingly
            if (auto* Settings = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ViewDistanceQuality")))
            {
                int32 ViewQuality = Settings->GetInt();
                if (ViewQuality <= 1) TargetLOD = 2;      // Low quality - use LOD 2
                else if (ViewQuality <= 2) TargetLOD = 1; // Medium quality - use LOD 1
                else TargetLOD = 0;                       // High/Epic - use LOD 0
            }
            
            GroomComp->SetForcedLOD(TargetLOD);
            
            // Reduce simulation quality for performance
            if (TargetLOD > 0)
            {
                GroomComp->SetEnableSimulation(false); // Disable simulation on lower quality
            }
            
            // Disable distance-based culling
            GroomComp->SetCullDistance(0.0f);
            
            // Ensure visibility
            GroomComp->SetVisibility(true, true);
            
            UE_LOG(LogTemp, Warning, TEXT("Set LOD %d for groom: %s"), TargetLOD, *GroomComp->GetName());
        }
    }
}

// === Deprecated Ground Positioning Functions (Characters now spawn at fixed Z=50) ===
void ACharacterRaceLineup::PositionCharactersOnGround()
{
    UE_LOG(LogTemp, Warning, TEXT("PositionCharactersOnGround: Characters now spawn at fixed Z=50, no ground positioning needed"));
}

float ACharacterRaceLineup::GetGroundHeightAtLocation(FVector Location)
{
    UE_LOG(LogTemp, Warning, TEXT("GetGroundHeightAtLocation: Characters now spawn at fixed Z=50, ground detection removed"));
    return 50.0f; // Fixed character height
}

void ACharacterRaceLineup::DebugGroundTracing()
{
    UE_LOG(LogTemp, Warning, TEXT("DebugGroundTracing: Ground tracing removed, characters spawn at fixed Z=50"));
}