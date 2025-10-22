#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Camera/CameraComponent.h"
#include "Characters/Creation/CharacterCreatorDataAsset.h"
#include "GroomComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/PostProcessVolume.h"
#include "CharacterRaceLineup.generated.h"

class UAnimInstance;
struct FRacePreset;

UCLASS()
class DAWNBLADE_API ACharacterRaceLineup : public AActor
{
    GENERATED_BODY()

public:
    ACharacterRaceLineup();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // === Data ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select")
    UCharacterCreatorDataAsset* CreatorDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select")
    int32 CurrentIndex = 0;

    // Characters & components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race Select")
    TArray<TObjectPtr<USkeletalMeshComponent>> SpawnedCharacters;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race Select")
    TArray<TObjectPtr<UGroomComponent>> SpawnedHairGrooms;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race Select")
    TArray<TObjectPtr<USpotLightComponent>> CharacterSpotlights;

    // === NEW: Hair Performance Settings ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hair Performance")
    bool bEnableHairSimulationOnlyForSelected = true; // Major performance boost

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hair Performance")
    bool bUseDynamicHairLOD = true; // Auto-adjust hair quality based on selection

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hair Performance")
    int32 SelectedCharacterHairLOD = 0; // Best quality for selected

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hair Performance")
    int32 BackgroundCharacterHairLOD = 2; // Lower quality for background

    // === NEW: Character Positioning ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Positioning")
    float GroundOffset = -5.f; // How high above ground to place characters (negative to lower them)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Positioning")
    bool bEnableFootIK = true; // Enable foot IK positioning

    // Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race Select|Camera")
    TObjectPtr<UCameraComponent> LineupCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Camera")
    float CameraDistance = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Camera")
    float CameraHeight = 125.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Camera")
    float CameraLerpSpeed = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Camera")
    float CameraFOV = 75.f;

    // Fallback animation blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Animation")
    TSubclassOf<UAnimInstance> FallbackAnimBP;

    // === Enhanced Lighting ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Lighting")
    float LightTransitionSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Lighting")
    float MainLightIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Lighting")
    FLinearColor MainLightColor = FLinearColor(1.0f, 0.9f, 0.8f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Lighting")
    float CharacterSpotlightIntensity = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select|Lighting")
    float RimLightIntensity = 300.0f;

    UPROPERTY()
    TArray<float> TargetSpotlightIntensities;

    UPROPERTY()
    float LastSelectionTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race Select")
    float SelectionCooldown = 0.5f;

    UPROPERTY()
    int32 DebugFrameCounter = 0;

    UFUNCTION()
    void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

    UFUNCTION(BlueprintCallable, Category="Debug")
    void TestMontageOnCurrentCharacter();

    // === Functions ===
    UFUNCTION(BlueprintCallable, Category="Race Select")
    void ShowRaceAtIndex(int32 Index);

    UFUNCTION(BlueprintCallable, Category="Race Select")
    void NextRace();

    UFUNCTION(BlueprintCallable, Category="Race Select")
    void PrevRace();

    UFUNCTION(BlueprintCallable, Category="Race Select")
    FRacePreset GetCurrentRace() const;

    // Hair management
    UFUNCTION(BlueprintCallable, Category="Race Select|Hair")
    void RefreshAllHair();

    UFUNCTION(BlueprintCallable, Category="Race Select|Hair")
    void SetHairVisibility(bool bVisible);

    // === NEW: Sci-Fi Environment Functions ===
    UFUNCTION(BlueprintCallable, Category="Sci-Fi Environment")
    void SetupSciFiEnvironment();

    UFUNCTION(BlueprintCallable, Category="Sci-Fi Environment")
    void SpawnFloatingOrbs();

    UFUNCTION(BlueprintCallable, Category="Sci-Fi Environment")
    void AnimateSciFiElements(float DeltaTime);

    // === NEW: Environment Functions ===
    UFUNCTION(BlueprintCallable, Category="Cinematic Environment")
    void SetupCinematicEnvironment();

    UFUNCTION(BlueprintCallable, Category="Cinematic Environment")
    void SpawnSmokeEffects();

    UFUNCTION(BlueprintCallable, Category="Cinematic Environment")
    void UpdateEnvironmentalEffects(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category="Cinematic Environment")
    void SetCinematicLighting();

    // === Hair Management Enhanced ===
    UFUNCTION(BlueprintCallable, Category="Race Select|Hair")
    void ForceAllHairVisible();

    UFUNCTION(BlueprintCallable, Category="Race Select|Hair")
    void DisableHairLODForAllCharacters();

    // === NEW: Character Positioning Functions ===
    UFUNCTION(BlueprintCallable, Category="Character Positioning")
    void PositionCharactersOnGround();

    UFUNCTION(BlueprintCallable, Category="Character Positioning")
    float GetGroundHeightAtLocation(FVector Location);

    UFUNCTION(BlueprintCallable, Category="Debug")
    void DebugGroundTracing();

private:
    double LoadStartTime = 0.0; 

    // === Async loading hooks (future use) ===
    void StartAsyncAssetLoad();
    void OnAssetsLoaded();
    void SpawnAllCharacters();

    // === Core helpers ===
    void UpdateCameraTarget();
    void AttachRaceHairToCharacter(int32 CharacterIndex, USkeletalMeshComponent* BodyComponent, const FRacePreset& RacePreset);
    void CleanupHairForCharacter(int32 CharacterIndex);

    // Camera state
    FVector CameraTargetLocation;
    FRotator CameraTargetRotation;

    // Cinematic camera positions
    FVector DesiredCameraTargetLocation;
    FRotator DesiredCameraTargetRotation;
};