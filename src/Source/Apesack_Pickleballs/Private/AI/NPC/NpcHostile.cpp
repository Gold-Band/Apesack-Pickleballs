// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcHostile.h"

#include "AI/HTN/HTNComponent.h"
#include "Managers/NpcManager.h"

// Sets default values
ANpcHostile::ANpcHostile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(&TargetNearestAttackableAction);
	MoveToAction.bUseLineOfSight = true;
	MeleeAttackTask.Actions.Add(&MoveToAction);
	MeleeAttackTask.Actions.Add(&MeleeAttackAction);
	MeleeAttackTask.Actions.Add(&CooldownAction);
	HtnDomain->AssignTask(MeleeAttackTask);
}

void ANpcHostile::BeginPlay()
{
	Super::BeginPlay();

	if (NpcManager) NpcManager->AddNpc(this, ENpcTag::Hostile);
}

void ANpcHostile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NpcManager) NpcManager->RemoveNpc(this, ENpcTag::Hostile);
	Super::EndPlay(EndPlayReason);
}
