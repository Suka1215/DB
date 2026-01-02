// DBAIController_Enemy.cpp
#include "DBAIController_Enemy.h"

#include "StateTree.h"
#include "Components/DBAIStateComponent.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Enemy/Data/DEnemyData.h"
#include "Kismet/GameplayStatics.h"
#include "Types/DBEnemyStateTreeContext.h"

ADBAIController_Enemy::ADBAIController_Enemy()
{
    // Create StateTree Component
    StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
    StateTreeComponent->SetAutoActivate(false);  // ✅ ADD THIS LINE
    StateTreeContext = CreateDefaultSubobject<UDBEnemyStateTreeContext>(TEXT("StateTreeContext"));


    // Create Perception Component
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->ConfigureSense(*DamageConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    // Initialize variables
    CurrentTarget = nullptr;
    LastKnownTargetLocation = FVector::ZeroVector;
    HomeLocation = FVector::ZeroVector;
}

void ADBAIController_Enemy::BeginPlay()
{
    Super::BeginPlay();
    SetupPerception();
}

void ADBAIController_Enemy::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    // 🔥 StateTree should run ONLY on the server
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Log, TEXT("🚫 %s: Client - skipping AI init"), *GetName());
        return;
    }
    
    ADBEnemyCharacter* EnemyChar = Cast<ADBEnemyCharacter>(InPawn);
    if (!EnemyChar) return;

    // Defer setup until EnemyData is initialized
    if (!EnemyChar->GetEnemyData())
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ EnemyData not yet initialized for %s, deferring setup..."), *EnemyChar->GetName());
        FTimerHandle TempHandle;
        GetWorld()->GetTimerManager().SetTimer(TempHandle, [this, EnemyChar]()
        {
            if (EnemyChar && EnemyChar->GetEnemyData())
            {
                UE_LOG(LogTemp, Warning, TEXT("🔁 Completing deferred AI setup for %s"), *EnemyChar->GetName());
                // ✅ Just call the initialization directly, don't re-possess!
                InitializeAI(EnemyChar);
            }
        }, 0.1f, false);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("🤖 AI Controller possessed: %s"), *EnemyChar->GetName());

    // Set home location to spawn point
    HomeLocation = EnemyChar->GetActorLocation();

    // ✅ Call our new initialization method
    InitializeAI(EnemyChar);
}

void ADBAIController_Enemy::InitializeAI(ADBEnemyCharacter* EnemyChar)
{
    if (!HasAuthority()) return;
    if (!EnemyChar)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ InitializeAI FAILED: EnemyChar is NULL"));
        return;
    }

    UDEnemyData* EnemyData = EnemyChar->GetEnemyData();
    if (!EnemyData)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠ EnemyData not replicated yet, retrying..."));
        // retry shortly
        FTimerHandle Retry;
        GetWorld()->GetTimerManager().SetTimer(Retry, [this, EnemyChar]()
        {
            InitializeAI(EnemyChar);
        }, 0.15f, false);
        return;
    }

    CachedEnemyData = EnemyData;
    bShowDebugLines = EnemyData->bShowDebugLines;

    // ------------------------------------------
    // 🧠 Assign StateTree (UE5.7 compatible)
    // ------------------------------------------
    UStateTree* StateTreeToUse = EnemyData->StateTreeAsset.Get();    // <-- Fix #1

    if (!StateTreeToUse && DefaultStateTree)
        StateTreeToUse = DefaultStateTree;

    if (!StateTreeToUse)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No StateTree assigned to EnemyData or DefaultStateTree."));
        return;
    }

    StateTreeComponent->SetStateTree(StateTreeToUse);

    // 🚨 UE5.7 DOES NOT SUPPORT registering external data in code anymore.
    // Your StateTree asset MUST declare:
    //   - external ADBAIController_Enemy*
    //   - external ADBEnemyCharacter*
    //   - external UDBEnemyStateTreeContext*
    //
    // (Instructions below)

    if (!StateTreeComponent->IsRunning())
    {
        StateTreeComponent->StartLogic();
        UE_LOG(LogTemp, Warning, TEXT("🔥 StateTree STARTED — %s"), *StateTreeToUse->GetName());
    }

    // ------------------------------------------
    // Sight configuration stays the same
    // ------------------------------------------
    if (SightConfig)
    {
        switch (EnemyData->DetectionMethod)
        {
        case EDetectionType::Sight:
            SightConfig->SightRadius = EnemyData->SightRange;
            SightConfig->PeripheralVisionAngleDegrees = EnemyData->VisionAngle;
            break;
        case EDetectionType::Sound:
            SightConfig->SightRadius = 0.f;
            break;
        case EDetectionType::TrueSight:
            SightConfig->SightRadius = EnemyData->TrueSightRange;
            SightConfig->PeripheralVisionAngleDegrees = 180.f;
            SightConfig->SetMaxAge(0.f);
            break;
        }

        AIPerception->RequestStimuliListenerUpdate();
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ AI initialized successfully for %s"), *EnemyChar->GetName());
}



void ADBAIController_Enemy::OnUnPossess()
{
    // Clean up
    CurrentTarget = nullptr;
    LastKnownTargetLocation = FVector::ZeroVector;

    Super::OnUnPossess();
}

void ADBAIController_Enemy::SetupPerception()
{
    if (AIPerception)
    {
        AIPerception->OnPerceptionUpdated.AddDynamic(this, &ADBAIController_Enemy::OnPerceptionUpdated);
        AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ADBAIController_Enemy::OnTargetPerceived);
    }
}

void ADBAIController_Enemy::SetTargetActor(AActor* NewTarget)
{
    CurrentTarget = NewTarget;
    
    // ✅ Update StateTree context
    if (StateTreeContext)
    {
        StateTreeContext->CurrentTarget = NewTarget;
        StateTreeContext->bIsEngaged = (NewTarget != nullptr);
        
        if (NewTarget)
        {
            ADBEnemyCharacter* EnemyChar = GetEnemyCharacter();
            if (EnemyChar)
            {
                StateTreeContext->UpdateDistanceToTarget(EnemyChar->GetActorLocation());
                
                UE_LOG(LogTemp, Warning, TEXT("🎯 Target set to: %s (Distance: %.1f)"), 
                    *NewTarget->GetName(), 
                    StateTreeContext->DistanceToTarget);
            }
        }
    }

    if (NewTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎯 Target set to: %s"), *NewTarget->GetName());
    }
}

void ADBAIController_Enemy::UpdateLastKnownTargetLocation()
{
    if (CurrentTarget)
    {
        LastKnownTargetLocation = CurrentTarget->GetActorLocation();
    }
}

void ADBAIController_Enemy::ClearTarget()
{
    CurrentTarget = nullptr;
    
    // ✅ Update StateTree context
    if (StateTreeContext)
    {
        StateTreeContext->ClearTarget();
        StateTreeContext->bIsEngaged = false;
    }
    
    // UE_LOG(LogTemp, Log, TEXT("🎯 Target cleared"));
}

void ADBAIController_Enemy::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    // Called when perception updates
}

void ADBAIController_Enemy::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;

    // ✅ FILTER: Skip other enemies!
    if (Actor->ActorHasTag(FName("Enemy")))
    {
        return;
    } 

    // ✅ FILTER: Only target players
    if (!Actor->ActorHasTag(FName("Player")) && !Cast<APawn>(Actor)->IsPlayerControlled())
    {
        return;
    }

    ADBEnemyCharacter* EnemyChar = GetEnemyCharacter();
    if (!EnemyChar || !EnemyChar->GetEnemyData()) return;

    UDEnemyData* EnemyData = EnemyChar->GetEnemyData();
    ACharacter* PlayerChar = Cast<ACharacter>(Actor);
    if (!PlayerChar) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        float Distance = FVector::Dist(EnemyChar->GetActorLocation(), Actor->GetActorLocation());
        
        // ✅ Debug visualization
        DrawDetectionDebug(EnemyChar, Actor, Distance);
        
        UE_LOG(LogTemp, Warning, TEXT("👁️ %s detected player! (Distance: %.1f, Method: %d)"), 
            *EnemyData->EnemyName.ToString(), 
            Distance,
            (int32)EnemyData->DetectionMethod);

        bool bShouldEngage = false;

        switch (EnemyData->Behavior)
        {
            case EEnemyBehavior::Passive:
                bShouldEngage = false;
                break;

            case EEnemyBehavior::Defensive:
                bShouldEngage = Distance <= EnemyData->AggroRange;
                break;

            case EEnemyBehavior::Aggressive:
                bShouldEngage = true;
                break;
                
            case EEnemyBehavior::Linking:
                bShouldEngage = true;
                CallNearbyAllies(EnemyData->LinkRange);
                break;
        }

        if (bShouldEngage)
        {
            SetTargetActor(Actor);

            UE_LOG(LogTemp, Warning, TEXT("⚔️ %s engaging player!"), *EnemyData->EnemyName.ToString());

            // ✅ Update tags
            if (UDBAIStateComponent* TagState = EnemyChar->FindComponentByClass<UDBAIStateComponent>())
            {
                TagState->RemoveWeapon_Sheathed();
                TagState->RemoveAI_Patrolling();
                TagState->AddAI_Engaged();
                UE_LOG(LogTemp, Log, TEXT("📡 Tags updated: Weapon Drawn / Engaged true"));
            }

            // ✅ FIRE STATE.AI.ENGAGED EVENT TO STATETREE
            if (StateTreeComponent && StateTreeComponent->IsRunning())
            {
                FGameplayTag EngagedTag = FGameplayTag::RequestGameplayTag(FName("State.AI.Engaged"));
                StateTreeComponent->SendStateTreeEvent(EngagedTag);
                
                UE_LOG(LogTemp, Warning, TEXT("🔥 Fired StateTree event: State.AI.Engaged"));
            }
        }
    }
    else
    {
        if (CurrentTarget == Actor)
        {
            UE_LOG(LogTemp, Warning, TEXT("👁️ %s lost sight of player"), *EnemyData->EnemyName.ToString());

            UpdateLastKnownTargetLocation();
            ClearTarget();

            // ✅ Update tags
            if (UDBAIStateComponent* TagState = EnemyChar->FindComponentByClass<UDBAIStateComponent>())
            {
                TagState->RemoveWeapon_Drawn();
                TagState->SetWeapon_Sheathed();
                TagState->RemoveAI_Patrolling();
                TagState->RemoveAI_Engaged();
                UE_LOG(LogTemp, Log, TEXT("📡 Tags updated: Weapon Sheathed / Engaged false"));
            }

            // ✅ FIRE STATE.AI.DISENGAGED EVENT TO STATETREE
            if (StateTreeComponent && StateTreeComponent->IsRunning())
            {
                FGameplayTag DisengagedTag = FGameplayTag::RequestGameplayTag(FName("State.AI.Disengaged"));
                StateTreeComponent->SendStateTreeEvent(DisengagedTag);
                
                UE_LOG(LogTemp, Warning, TEXT("🔥 Fired StateTree event: State.AI.Disengaged"));
            }
        }
    }
}

void ADBAIController_Enemy::CallNearbyAllies(float LinkRadius)
{
    ADBEnemyCharacter* EnemyChar = GetEnemyCharacter();
    if (!EnemyChar || !CurrentTarget) return;

    TArray<AActor*> NearbyEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADBEnemyCharacter::StaticClass(), NearbyEnemies);

    int32 AlliesCalled = 0;
    FVector MyLocation = EnemyChar->GetActorLocation();
    
    for (AActor* Actor : NearbyEnemies)
    {
        ADBEnemyCharacter* Ally = Cast<ADBEnemyCharacter>(Actor);
        if (!Ally || Ally == EnemyChar) continue; // Skip self
        
        float Distance = FVector::Dist(MyLocation, Ally->GetActorLocation());
        
        if (Distance <= LinkRadius)
        {
            // Get the ally's AI controller
            if (ADBAIController_Enemy* AllyController = Cast<ADBAIController_Enemy>(Ally->GetController()))
            {
                // Only call if they don't already have a target
                if (!AllyController->GetCurrentTarget())
                {
                    AllyController->SetTargetActor(CurrentTarget);
                    AlliesCalled++;
                    
                    // ✅ Debug line to linked ally
                    if (bShowDebugLines)
                    {
                        DrawDebugLine(
                            GetWorld(),
                            MyLocation + FVector(0, 0, 90),
                            Ally->GetActorLocation() + FVector(0, 0, 90),
                            FColor::Orange,
                            false,
                            3.0f,
                            0,
                            5.0f
                        );
                    }
                    
                    UE_LOG(LogTemp, Warning, TEXT("📢 %s called ally %s to combat!"), 
                        *EnemyChar->GetName(), 
                        *Ally->GetName());
                }
            }
        }
    }
    
    if (AlliesCalled > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("📢 %s linked %d allies!"), *EnemyChar->GetName(), AlliesCalled);
    }
}

void ADBAIController_Enemy::DrawDetectionDebug(ADBEnemyCharacter* EnemyChar, AActor* Target, float Distance)
{
    if (!bShowDebugLines || !EnemyChar || !Target) return;
    
    UDEnemyData* EnemyData = EnemyChar->GetEnemyData();
    if (!EnemyData) return;
    
    FVector EnemyLocation = EnemyChar->GetActorLocation();
    FVector EnemyLocationRaised = EnemyLocation + FVector(0, 0, 90);
    FVector TargetLocation = Target->GetActorLocation() + FVector(0, 0, 90);
    
    // ✅ Use manual circle drawing for solid lines
    auto DrawSolidCircle = [&](FVector Center, float Radius, FColor Color, float Thickness)
    {
        const int32 Segments = 64;
        FVector LastPoint = Center + FVector(Radius, 0, 0);
        
        for (int32 i = 1; i <= Segments; i++)
        {
            float Angle = (2.0f * PI * i) / Segments;
            FVector NewPoint = Center + FVector(
                FMath::Cos(Angle) * Radius,
                FMath::Sin(Angle) * Radius,
                0
            );
            
            DrawDebugLine(
                GetWorld(),
                LastPoint,
                NewPoint,
                Color,
                false,
                2.0f,
                0,
                Thickness
            );
            
            LastPoint = NewPoint;
        }
    };
    
    // Line to target
    DrawDebugLine(
        GetWorld(),
        EnemyLocationRaised,
        TargetLocation,
        FColor::Yellow,
        false,
        2.0f,
        0,
        1.5f
    );
    
    // ✅ Draw detection ranges based on detection method
    switch (EnemyData->DetectionMethod)
    {
        case EDetectionType::Sight:
        {
            // Green circle for sight range
            DrawSolidCircle(EnemyLocation, EnemyData->SightRange, FColor::Green, 1.5f);
            
            // Cyan vision cone edges
            FVector ForwardVector = EnemyChar->GetActorForwardVector();
            FVector RightEdge = ForwardVector.RotateAngleAxis(EnemyData->VisionAngle, FVector::UpVector);
            FVector LeftEdge = ForwardVector.RotateAngleAxis(-EnemyData->VisionAngle, FVector::UpVector);
            
            DrawDebugLine(
                GetWorld(),
                EnemyLocationRaised,
                EnemyLocationRaised + (RightEdge * EnemyData->SightRange),
                FColor::Cyan,
                false,
                2.0f,
                0,
                1.5f
            );
            
            DrawDebugLine(
                GetWorld(),
                EnemyLocationRaised,
                EnemyLocationRaised + (LeftEdge * EnemyData->SightRange),
                FColor::Cyan,
                false,
                2.0f,
                0,
                1.5f
            );
            break;
        }
        
        case EDetectionType::Sound:
        {
            // Purple circle for hearing range (360°)
            DrawSolidCircle(EnemyLocation, EnemyData->HearingRange, FColor::Purple, 1.5f);
            break;
        }
        
        case EDetectionType::TrueSight:
        {
            // Blue circle for true sight vision (360°)
            DrawSolidCircle(EnemyLocation, EnemyData->TrueSightRange, FColor::Blue, 1.5f);
            
            // Magenta circle for true sight hearing (360°)
            DrawSolidCircle(EnemyLocation, EnemyData->TrueSightHearingRange, FColor::Magenta, 1.5f);
            break;
        }
    }
    
    // Red circle for aggro range
    DrawSolidCircle(EnemyLocation, EnemyData->AggroRange, FColor::Red, 1.5f);
    
    // Distance text
    FString DistanceText = FString::Printf(TEXT("Distance: %.0f"), Distance);
    DrawDebugString(
        GetWorld(),
        EnemyLocationRaised + FVector(0, 0, 50),
        DistanceText,
        nullptr,
        FColor::White,
        2.0f
    );
}

ADBEnemyCharacter* ADBAIController_Enemy::GetEnemyCharacter() const
{
    return Cast<ADBEnemyCharacter>(GetPawn());
}