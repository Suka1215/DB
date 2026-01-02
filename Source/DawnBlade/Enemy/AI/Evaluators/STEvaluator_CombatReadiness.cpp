#include "STEvaluator_CombatReadiness.h"
#include "Enemy/DBEnemyCharacter.h"
#include "Enemy/AI/DBAIController_Enemy.h"
#include "Enemy/AI/Components/DBAIStateComponent.h"
#include "Enemy/Data/DEnemyData.h"

USTEvaluator_CombatReadiness::USTEvaluator_CombatReadiness()
{
}

void USTEvaluator_CombatReadiness::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
    Super::Tick(Context, DeltaTime);

    // ✅ Get AI Controller directly
    ADBAIController_Enemy* AIController = GetAIController(Context);
    
    if (!AIController)
    {
        bCanAttack = false;
        bIsAttacking = false;
        bIsArmed = false;
        bWeaponDrawn = false;
        bInCombat = false;
        TimeUntilNextAttack = 0.0f;
        bHasResources = false;
        return;
    }

    // ✅ Get cached enemy data from AI Controller
    UDEnemyData* EnemyData = AIController->GetCachedEnemyData();
    if (!EnemyData)
    {
        bCanAttack = false;
        bIsAttacking = false;
        bIsArmed = false;
        bWeaponDrawn = false;
        bInCombat = false;
        TimeUntilNextAttack = 0.0f;
        bHasResources = false;
        return;
    }

    // ✅ Get enemy character
    ADBEnemyCharacter* Enemy = GetEnemyCharacter(Context);
    if (!Enemy)
    {
        bCanAttack = false;
        bIsAttacking = false;
        bIsArmed = false;
        bWeaponDrawn = false;
        bInCombat = false;
        TimeUntilNextAttack = 0.0f;
        bHasResources = false;
        return;
    }

    // ✅ Now safe to use enemy data from controller
    bIsArmed = Enemy->bIsArmed;

    // Check if weapon is drawn (dynamic state)
    UDBAIStateComponent* StateComp = Enemy->FindComponentByClass<UDBAIStateComponent>();
    if (StateComp)
    {
        bWeaponDrawn = StateComp->HasTag(FName("Weapon.Drawn"));
        bInCombat = StateComp->HasTag(FName("State.AI.Engaged"));
    }
    else
    {
        bWeaponDrawn = false;
        bInCombat = false;
    }

    // Check if currently attacking
    bIsAttacking = Enemy->bIsAttacking;

    // Check attack cooldown
    TimeUntilNextAttack = FMath::Max(0.0f, Enemy->NextAttackTime - Enemy->GetWorld()->GetTimeSeconds());
    
    // Can attack if armed, weapon drawn, not attacking, and cooldown ready
    bCanAttack = bIsArmed && bWeaponDrawn && !bIsAttacking && TimeUntilNextAttack <= 0.0f;

    // Check resources
    bHasResources = true;

    UE_LOG(LogTemp, VeryVerbose, TEXT("Combat Readiness: HasWeapon=%s | Drawn=%s | CanAttack=%s | IsAttacking=%s | Cooldown=%.1fs"),
        bIsArmed ? TEXT("YES") : TEXT("NO"),
        bWeaponDrawn ? TEXT("YES") : TEXT("NO"),
        bCanAttack ? TEXT("YES") : TEXT("NO"),
        bIsAttacking ? TEXT("YES") : TEXT("NO"),
        TimeUntilNextAttack
    );
}