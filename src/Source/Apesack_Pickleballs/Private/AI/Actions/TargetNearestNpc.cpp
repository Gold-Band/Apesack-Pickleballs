#include "AI/Actions/TargetNearestNpc.h"
#include "AI/NPC/Npc.h"
#include "Managers/NpcManager.h"

FTargetNearestNpcAction::FTargetNearestNpcAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Target Npc";
	bIsFirstCall = true;
	NearestNpc = nullptr;
	NearestType = ENpcSearchOption::Any;
}

bool FTargetNearestNpcAction::IsExecutable() const
{
	return UNpcManager::Get(Owner->GetWorld())->SenseNpc(Owner->GetActorLocation(), NearestType, Owner->SenseRadius);
}

void FTargetNearestNpcAction::Execute(float DeltaTime)
{
	if (bIsFirstCall)
	{
		OnFirstExecute();	
	}
	
	if (NearestNpc != nullptr) State = EActionState::Succeeded;
	else State = EActionState::Failed;
}

void FTargetNearestNpcAction::Reset()
{
	FAction::Reset();
	bIsFirstCall = true;	
}

void FTargetNearestNpcAction::OnFirstExecute()
{
	bIsFirstCall = false;	
	const UWorld* World = Owner->GetWorld();
	const FVector Location = Owner->GetActorLocation();
	UNpcManager* NpcManager = UNpcManager::Get(World);
	NearestNpc = Owner->TargetActor = NpcManager->FindNearestNpc(Location, NearestType);
}