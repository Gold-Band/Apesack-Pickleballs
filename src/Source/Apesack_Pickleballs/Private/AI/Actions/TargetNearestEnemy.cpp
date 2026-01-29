#include "AI/Actions/TargetNearestEnemy.h"
#include "AI/NPC/Npc.h"
#include "Managers/NpcManager.h"

FTargetNearestEnemyAction::FTargetNearestEnemyAction(ANpc* OwnerNpc) : FTargetNearestNpcAction(OwnerNpc)
{
	Name = "Target Enemy";
	NearestType = ENpcSearchOption::AnyHostile;
}

void FTargetNearestEnemyAction::OnFirstExecute()
{
	FTargetNearestNpcAction::OnFirstExecute();
	if (Owner->bPrintDebug_TargetNearestEnemy) UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s  Owner->TargetActor = %s"), NearestNpc? TEXT("Valid"): TEXT("Null"), Owner->TargetActor? TEXT("Valid") : TEXT("Null"));
}
