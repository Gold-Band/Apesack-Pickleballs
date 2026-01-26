// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/Actions/OccupyTower.h"

#include "AI/NPC/Npc.h"
#include "Buildings/ArcherTower.h"

FOccupyTowerAction::FOccupyTowerAction(ANpc* OwnerNpc)
{
	Name = "Occupy Tower";
	Owner = OwnerNpc;
}

bool FOccupyTowerAction::IsExecutable() const
{
	return Owner->CharacterClass == ECharacterType::Archer;
}

void FOccupyTowerAction::Execute(float DeltaTime)
{
	AArcherTower* TargetTower = Cast<AArcherTower>(Owner->TargetActor);
	if (!TargetTower->HasRoom())
	{
		State = EActionState::Failed;
		return;
	}
	
	if (Owner->bCanMove)
	{
		TargetTower->AddOccupant(Owner);
		Owner->bCanMove = false;
	}
	State = EActionState::Succeeded;
}

void FOccupyTowerAction::Reset()
{
	FAction::Reset();
}
