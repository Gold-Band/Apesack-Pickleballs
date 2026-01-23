#include "AI/Actions/MoveTimed.h"
#include "AI/NPC/Npc.h"

FMoveTimedAction::FMoveTimedAction(ANpc* OwnerNpc)
{
	Name = "Move Timed";
	Owner = OwnerNpc;
	MoveTime = 0;
	ExecutionTime = 0;
	Direction = 0;
	bPrintDebug = false;
}

bool FMoveTimedAction::IsExecutable() const
{
	return true;
}

void FMoveTimedAction::Execute(float DeltaTime)
{
	if (MoveTime == 0)
	{
		MoveTime = FMath::RandRange(Owner->WanderTimeMin, Owner->WanderTimeMax);
		Direction = FMath::RandRange(-1, 1);
		if (bPrintDebug) UE_LOG(LogTemp, Warning, TEXT("set move time to %fs"), MoveTime);
	}
	
	Owner->MoveForwardScaled(Direction * Owner->WanderSpeed);
	
	ExecutionTime += DeltaTime;
	if (ExecutionTime >= MoveTime) State = EActionState::Succeeded;
}

void FMoveTimedAction::Reset()
{
	FAction::Reset();
	MoveTime = 0;
	ExecutionTime = 0;
}
