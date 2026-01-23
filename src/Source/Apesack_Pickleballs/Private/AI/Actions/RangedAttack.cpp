#include "AI/Actions/RangedAttack.h"
#include "AI/NPC/Npc.h"

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
	//* 2. Get my statsComponent and pass some information down to the arrow
	//* 3. Call Launch At
	//*
	
	
	
	//const float Damage = Owner->GetStats()->GetRangedDamage(Owner->BaseDamage_MeleeAttack);
	
	State = EActionState::Succeeded;
	Owner->Delay = Owner->Cooldown_RangedAttack;
}

void FRangedAttackAction::Reset()
{
	FAction::Reset();
}
