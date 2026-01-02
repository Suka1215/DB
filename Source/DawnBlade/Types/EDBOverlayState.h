// ============================================================================
// DBTypes.h (or create new file: DBAnimationTypes.h)
// Add this enum to your types header
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Types/EDBJob.h"
#include "EDBOverlayState.generated.h"

/**
 * Overlay states for animation system
 * Determines which animation set to use based on equipment/combat state
 */
UENUM(BlueprintType)
enum class EDBOverlayState : uint8
{
	// No weapons equipped - normal movement
	Default         UMETA(DisplayName = "Default"),
    
	// Sword and Shield combat stance
	SwordAndShield  UMETA(DisplayName = "Sword and Shield"),
    
	// Two-handed weapon combat stance
	TwoHanded       UMETA(DisplayName = "Two-Handed"),
    
	// Dual wielding combat stance
	DualWield       UMETA(DisplayName = "Dual Wield"),
    
	// Bow/ranged weapon stance
	Bow             UMETA(DisplayName = "Bow"),
    
	// Magic/spellcasting stance
	Magic           UMETA(DisplayName = "Magic"),
    
	// Unarmed combat
	Unarmed         UMETA(DisplayName = "Unarmed"),
    
	// Add more as needed per job
	MAX             UMETA(Hidden)
};

/**
 * Helper struct to map jobs to their default overlay states
 */
USTRUCT(BlueprintType)
struct FJobOverlayMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDBJob Job = EDBJob::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDBOverlayState OverlayState = EDBOverlayState::Default;
};