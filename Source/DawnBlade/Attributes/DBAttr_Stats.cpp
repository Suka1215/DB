#include "Attributes/DBAttr_Stats.h"
#include "Net/UnrealNetwork.h"

UDBAttr_Stats::UDBAttr_Stats()
{
	// Set both base and current values so replication + gameplay math work properly
	Health.SetBaseValue(1200.f);
	Health.SetCurrentValue(1200.f);

	MaxHealth.SetBaseValue(3000.f);
	MaxHealth.SetCurrentValue(3000.f);

	Mana.SetBaseValue(1100.f);
	Mana.SetCurrentValue(1100.f);

	MaxMana.SetBaseValue(1500.f);
	MaxMana.SetCurrentValue(1500.f);

	TP.SetBaseValue(300.f);
	TP.SetCurrentValue(300.f);

	MaxTP.SetBaseValue(800.f);
	MaxTP.SetCurrentValue(800.f);
}

void UDBAttr_Stats::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, Health,    COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, Mana,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, MaxMana,   COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, TP,        COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDBAttr_Stats, MaxTP,     COND_None, REPNOTIFY_Always);
}
