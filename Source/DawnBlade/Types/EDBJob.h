// ============================================================================
// EDBJob.h - UPDATED WITH ALL YOUR JOBS
// ============================================================================
// Replace your existing /Source/DawnBlade/Types/EDBJob.h with this version
// ============================================================================

#pragma once

UENUM(BlueprintType)
enum class EDBJob : uint8
{
	None UMETA(DisplayName = "None"),
    
	// Starting Jobs (Currently Implemented)
	LightMage UMETA(DisplayName = "Light Mage"),
	Warrior UMETA(DisplayName = "Warrior"),
	Paladin UMETA(DisplayName = "Paladin"),
    
	// Starting Jobs (To Be Implemented)7
	Magus UMETA(DisplayName = "Magus"),
	Ninja UMETA(DisplayName = "Ninja"),
	Bandit UMETA(DisplayName = "Bandit"),
	Elder UMETA(DisplayName = "Elder"),
    
	// Advanced Jobs (Future)
	// Sentinel UMETA(DisplayName = "Sentinel"),
};