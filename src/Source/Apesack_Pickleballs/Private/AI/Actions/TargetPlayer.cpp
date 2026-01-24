#include "AI/Actions/TargetPlayer.h"
#include "AI/NPC/Npc.h"

FTargetPlayerAction::FTargetPlayerAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Target Player";
	bIsFirstCall = true;
	bPrintDebug = false;
	TargetActor = nullptr;
}

bool FTargetPlayerAction::IsExecutable() const
{
	return true;
}

void FTargetPlayerAction::Execute(float DeltaTime)
{
	if (bIsFirstCall)
	{
		bIsFirstCall = false;
		if (bPrintDebug) UE_LOG(LogTemp, Warning, TEXT("Getting the player"));
		Owner->TargetActor = Owner->GetWorld()->GetFirstPlayerController()->GetPawn();
		TargetActor = Owner->TargetActor;
	}
	
	if (TargetActor != nullptr) State = EActionState::Succeeded;
	else State = EActionState::Failed;
}

void FTargetPlayerAction::Reset()
{
	FAction::Reset();
	bIsFirstCall = true;	
}
