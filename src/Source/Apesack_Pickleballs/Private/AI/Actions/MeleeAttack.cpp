#include "AI/Actions/MeleeAttack.h"

#include "StatsComponent.h"
#include "AI/NPC/Npc.h"

FMeleeAttackAction::FMeleeAttackAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Melee Attack";
}

bool FMeleeAttackAction::IsExecutable() const
{
	return FAction::IsExecutable(); // if we are armed with a melee weapon
}

void FMeleeAttackAction::Execute(float DeltaTime)
{
	// get target
	const AActor* Target = Owner->TargetActor;
	if (Target == nullptr)
	{
		State = EActionState::Failed;
		return;
	}
	
	// get target's stat component
	UStatsComponent* TargetStatComponent = Owner->TargetActor->GetComponentByClass<UStatsComponent>();
	if (TargetStatComponent == nullptr)
	{
		State = EActionState::Failed;
		return;		
	}
	
	const float Damage = Owner->GetStats()->GetMeleeDamage(Owner->BaseDamage_MeleeAttack);
	TargetStatComponent->ApplyDamagePatch(Damage);
	
	State = EActionState::Succeeded;
	Owner->Delay = Owner->Cooldown_MeleeAttack;
}

void FMeleeAttackAction::Reset()
{
	FAction::Reset();
}
