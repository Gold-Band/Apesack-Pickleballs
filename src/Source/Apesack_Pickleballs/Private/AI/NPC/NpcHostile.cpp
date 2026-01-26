// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcHostile.h"
#include "Managers/NpcManager.h"

// Sets default values
ANpcHostile::ANpcHostile()
{
	PrimaryActorTick.bCanEverTick = false;
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
