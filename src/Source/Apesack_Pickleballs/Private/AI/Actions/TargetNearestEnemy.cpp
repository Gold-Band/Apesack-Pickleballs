#include "AI/Actions/TargetNearestEnemy.h"
#include "AI/NPC/Npc.h"
#include "Managers/NpcManager.h"

FTargetNearestEnemyAction::FTargetNearestEnemyAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Target Player";
	bIsFirstCall = true;
	bPrintDebug = false;
	NearestEnemy = nullptr;
}

bool FTargetNearestEnemyAction::IsExecutable() const
{
	return UNpcManager::Get(Owner->GetWorld())->SenseNpc(Owner->GetActorLocation(), ENpcSearchOption::AnyHostile, 0.0f);
}

void FTargetNearestEnemyAction::Execute(float DeltaTime)
{
	if (bIsFirstCall)
	{
		bIsFirstCall = false;	
		const UWorld* World = Owner->GetWorld();
		const FVector Location = Owner->GetActorLocation();
		UNpcManager* NpcManager = UNpcManager::Get(World);
		NearestEnemy = Owner->TargetActor = NpcManager->FindNearestNpc(Location, ENpcSearchOption::AnyHostile);
		
		if (bPrintDebug) UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s  Owner->TargetActor = %s"), NearestEnemy? TEXT("Valid"): TEXT("Null"), Owner->TargetActor? TEXT("Valid") : TEXT("Null"));
	}
	
	if (NearestEnemy != nullptr) State = EActionState::Succeeded;
	else State = EActionState::Failed;
}

void FTargetNearestEnemyAction::Reset()
{
	FAction::Reset();
	bIsFirstCall = true;	
}
