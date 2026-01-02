// DBMusicZone.cpp
#include "DBMusicZone.h"
#include "Components/BoxComponent.h"
#include "Components/DBMusicManagerComponent.h"
#include "DrawDebugHelpers.h"

ADBMusicZone::ADBMusicZone()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create zone volume
    ZoneVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneVolume"));
    RootComponent = ZoneVolume;
    ZoneVolume->SetBoxExtent(FVector(1000.0f, 1000.0f, 500.0f));
    ZoneVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ADBMusicZone::BeginPlay()
{
    Super::BeginPlay();

    ZoneVolume->OnComponentBeginOverlap.AddDynamic(this, &ADBMusicZone::OnZoneBeginOverlap);
    ZoneVolume->OnComponentEndOverlap.AddDynamic(this, &ADBMusicZone::OnZoneEndOverlap);

    UE_LOG(LogTemp, Log, TEXT("🎵 Music Zone '%s' initialized"), *ZoneName.ToString());

    // Debug visualization
    if (bShowDebugBounds)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), ZoneVolume->GetScaledBoxExtent(),
            FColor::Cyan, true, -1.0f, 0, 5.0f);
    }
}

void ADBMusicZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if it's the player entering
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (UDBMusicManagerComponent* MusicManager = OtherActor->FindComponentByClass<UDBMusicManagerComponent>())
        {
            UE_LOG(LogTemp, Log, TEXT("🎵 Player entered zone: %s"), *ZoneName.ToString());
            MusicManager->SetZoneMusic(ZoneMusicData);
        }
    }
}

void ADBMusicZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
    {
        UE_LOG(LogTemp, Log, TEXT("🎵 Player left zone: %s"), *ZoneName.ToString());
    }
}

#if WITH_EDITOR
void ADBMusicZone::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Update zone name from music data if available
    if (ZoneMusicData && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADBMusicZone, ZoneMusicData))
    {
        ZoneName = ZoneMusicData->ZoneName;
    }
}
#endif