// DBMusicManagerComponent.cpp
#include "DBMusicManagerComponent.h"
#include "Components/AudioComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Sound/SoundCue.h"

UDBMusicManagerComponent::UDBMusicManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentMusicState = EMusicState::Exploration;
}

void UDBMusicManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    // Create audio component
    MusicAudioComponent = NewObject<UAudioComponent>(this);
    MusicAudioComponent->RegisterComponent();
    MusicAudioComponent->bAutoActivate = false;
    MusicAudioComponent->bIsUISound = true; // Prevents attenuation
    MusicAudioComponent->SetVolumeMultiplier(1.0f);
    
    // ✅ ADD THIS - Enable looping
    if (CurrentZoneMusic && CurrentZoneMusic->bLoopMusic)
    {
        MusicAudioComponent->bAutoDestroy = false;
    }

    // Bind to weapon events for combat detection
    if (AActor* Owner = GetOwner())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
        {
            FGameplayTag WeaponDrawnTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn"));
            FGameplayTag WeaponSheathedTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Sheathed"));

            ASC->GenericGameplayEventCallbacks.FindOrAdd(WeaponDrawnTag).AddUObject(this, &UDBMusicManagerComponent::OnWeaponDrawn);
            ASC->GenericGameplayEventCallbacks.FindOrAdd(WeaponSheathedTag).AddUObject(this, &UDBMusicManagerComponent::OnWeaponSheathed);
            
            UE_LOG(LogTemp, Log, TEXT("🎵 Music Manager - Bound to weapon events successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("🎵 Music Manager - Could not find AbilitySystemComponent on owner!"));
        }
    }

    // Start with default exploration music
    if (DefaultExplorationMusic)
    {
        MusicAudioComponent->SetSound(DefaultExplorationMusic);
        MusicAudioComponent->FadeIn(2.0f);
    }

    UE_LOG(LogTemp, Log, TEXT("🎵 Music Manager initialized"));
}

void UDBMusicManagerComponent::SetZoneMusic(UDBZoneMusicData* NewZoneMusic)
{
    if (!NewZoneMusic)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎵 Attempted to set null zone music"));
        return;
    }

    CurrentZoneMusic = NewZoneMusic;
    UE_LOG(LogTemp, Log, TEXT("🎵 Zone music changed to: %s"), *NewZoneMusic->ZoneName.ToString());

    // Transition to appropriate music for current state
    USoundBase* NewMusic = GetMusicForState(CurrentMusicState);
    TransitionToMusic(NewMusic);
}

void UDBMusicManagerComponent::SetMusicState(EMusicState NewState)
{
    if (CurrentMusicState == NewState) return;

    UE_LOG(LogTemp, Log, TEXT("🎵 Music state changed: %d -> %d"), (int32)CurrentMusicState, (int32)NewState);
    CurrentMusicState = NewState;

    USoundBase* NewMusic = GetMusicForState(NewState);
    TransitionToMusic(NewMusic);
}

void UDBMusicManagerComponent::OnWeaponDrawn(const FGameplayEventData* Payload)
{
    UE_LOG(LogTemp, Log, TEXT("🎵 Weapon drawn - switching to battle music"));
    bInCombat = true;
    SetMusicState(EMusicState::Battle);
}

void UDBMusicManagerComponent::OnWeaponSheathed(const FGameplayEventData* Payload)
{
    UE_LOG(LogTemp, Log, TEXT("🎵 Weapon sheathed - switching to exploration music"));
    bInCombat = false;
    SetMusicState(EMusicState::Exploration);
}

void UDBMusicManagerComponent::TransitionToMusic(USoundBase* NewMusic)
{
    if (!MusicAudioComponent || !NewMusic) return;

    // Get fade times from zone data or use defaults
    float FadeOutTime = CurrentZoneMusic ? CurrentZoneMusic->FadeOutTime : 1.5f;
    float FadeInTime = CurrentZoneMusic ? CurrentZoneMusic->FadeInTime : 2.0f;
    bool bShouldLoop = CurrentZoneMusic ? CurrentZoneMusic->bLoopMusic : true;

    // Don't transition if already playing this music
    if (MusicAudioComponent->Sound == NewMusic && MusicAudioComponent->IsPlaying())
    {
        return;
    }

    // Fade out current music
    MusicAudioComponent->FadeOut(FadeOutTime, 0.0f);

    // Switch after fade completes
    FTimerHandle SwitchTimer;
    GetWorld()->GetTimerManager().SetTimer(SwitchTimer, [this, NewMusic, FadeInTime, bShouldLoop]()
    {
        if (MusicAudioComponent)
        {
            MusicAudioComponent->Stop();
            MusicAudioComponent->SetSound(NewMusic);
            
            // ✅ Set looping based on zone data
            if (USoundCue* SoundCue = Cast<USoundCue>(NewMusic))
            {
                // Sound Cues handle looping internally
            }
            else if (USoundWave* SoundWave = Cast<USoundWave>(NewMusic))
            {
                SoundWave->bLooping = bShouldLoop;
            }
            
            MusicAudioComponent->FadeIn(FadeInTime);
        }
    }, FadeOutTime, false);
}

USoundBase* UDBMusicManagerComponent::GetMusicForState(EMusicState State)
{
    if (!CurrentZoneMusic)
    {
        // Use fallback music
        return (State == EMusicState::Battle) ? DefaultBattleMusic : DefaultExplorationMusic;
    }

    switch (State)
    {
        case EMusicState::Exploration:
            return CurrentZoneMusic->ExplorationMusic ? CurrentZoneMusic->ExplorationMusic : DefaultExplorationMusic;

        case EMusicState::Battle:
            return CurrentZoneMusic->BattleMusic ? CurrentZoneMusic->BattleMusic : DefaultBattleMusic;

        case EMusicState::BossBattle:
            return CurrentZoneMusic->BossBattleMusic ? CurrentZoneMusic->BossBattleMusic : CurrentZoneMusic->BattleMusic;

        case EMusicState::Night:
            return CurrentZoneMusic->NightMusic ? CurrentZoneMusic->NightMusic : CurrentZoneMusic->ExplorationMusic;

        default:
            return DefaultExplorationMusic;
    }
}