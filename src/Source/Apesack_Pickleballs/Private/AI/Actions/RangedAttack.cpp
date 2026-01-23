#include "AI/Actions/RangedAttack.h"

#include "StatsComponent.h"
#include "AI/NPC/Npc.h"
#include "GameModes/DefaultGameMode.h"
#include "Projectiles/Arrow.h"

FRangedAttackAction::FRangedAttackAction(ANpc* OwnerNpc)
{
}

bool FRangedAttackAction::IsExecutable() const
{
	return FAction::IsExecutable(); // if we are armed with a ranged weapon
}

void FRangedAttackAction::Execute(float DeltaTime)
{
	// get target
	const AActor* Target = Owner->TargetActor;
	if (Target == nullptr)
	{
		State = EActionState::Failed;
		return;
	}
	
	//*
	//*	Shoot - Change into ranged attack.
	//* 1. Get an arrow from the gamemode
	AArrow* Arrow = Cast<AArrow>(Cast<ADefaultGameMode>(Owner->GetWorld()->GetAuthGameMode())->GetArrow());
	
	//* 2. Get my statsComponent and pass some information down to the arrow
	Arrow->Damage = Owner->GetStats()->GetRangedDamage(Owner->BaseDamage_MeleeAttack);
	
	//* 3. Call Launch At
	Arrow->LaunchAt(Owner->GetActorLocation(), Target->GetActorLocation());
	
	//*
	
	
	
	
	State = EActionState::Succeeded;
	Owner->Delay = Owner->Cooldown_RangedAttack;
}

void FRangedAttackAction::Reset()
{
	FAction::Reset();
}
