#include "AI/Actions/Cooldown.h"
#include "AI/NPC/Npc.h"

FCooldownAction::FCooldownAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Delay";
	Timer = 0.0f;
}

bool FCooldownAction::IsExecutable() const
{
	return FAction::IsExecutable();
}

void FCooldownAction::Execute(float DeltaTime)
{
	Timer += DeltaTime;
	if (Timer >= Owner->Delay) State = EActionState::Succeeded;
}

void FCooldownAction::Reset()
{
	FAction::Reset();
	Timer = 0.0f;
}
