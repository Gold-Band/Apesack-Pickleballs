#include "AI/Actions/RangedAttack.h"

#include "StatsComponent.h"
#include "AI/NPC/Npc.h"
#include "GameModes/DefaultGameMode.h"
#include "Projectiles/Arrow.h"

FRangedAttackAction::FRangedAttackAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Ranged Attack";
}

bool FRangedAttackAction::IsExecutable() const
{
	return Owner->CharacterClass == ECharacterType::Archer;
}

void FRangedAttackAction::Execute(float DeltaTime)
{
	// get target
	if (Owner->TargetActor == nullptr)
	{
		State = EActionState::Failed;
		return;
	}
	
	//*
	//*	Shoot - Change into ranged attack.
	//* 1. Get an arrow from the gamemode
	AArrow* Arrow = Cast<AArrow>(Cast<ADefaultGameMode>(Owner->GetWorld()->GetAuthGameMode())->GetArrow());
	if (!Arrow)
	{
		State = EActionState::Failed;
		return;
	}
	
	//* 2. Get my statsComponent and pass some information down to the arrow
	Arrow->Damage = Owner->GetStats()->GetRangedDamage(Owner->BaseDamage_MeleeAttack);
	
	//* 3. Call Launch At
	Arrow->LaunchAt(Owner, Owner->GetProjectileSpawnLocation(), Owner->TargetActor->GetActorLocation());
	
	//*
	State = EActionState::Succeeded;
	Owner->Delay = Owner->Cooldown_RangedAttack;
}

void FRangedAttackAction::Reset()
{
	FAction::Reset();
}
