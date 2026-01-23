// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcFriendly.h"
#include "AI/HTN/HTNComponent.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/NpcManager.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Follow 
	FollowTask.Actions.Add(&TargetPlayerAction);
	FollowTask.Actions.Add(&MoveToAction);
	//HtnDomain->AssignTask(FollowTask, 0);
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(&TargetNearestEnemyAction);
	MeleeAttackTask.Actions.Add(&MoveToAction);
	MeleeAttackTask.Actions.Add(&MeleeAttackAction);
	MeleeAttackTask.Actions.Add(&CooldownAction);
	HtnDomain->AssignTask(MeleeAttackTask);
	
	// Occupy Tower
	OccupyTowerTask.Actions.Add(&TargetFarthestTowerAction);
	OccupyTowerTask.Actions.Add(&MoveToAction);
	OccupyTowerTask.Actions.Add(&OccupyTowerAction);
	HtnDomain->AssignTask(OccupyTowerTask);
	
	
	// Ranged Attack
	RangedAttackTask.Actions.Add(&TargetNearestEnemyAction);
	RangedAttackTask.Actions.Add(&RangedAttackAction);
	RangedAttackTask.Actions.Add(&CooldownAction);
	HtnDomain->AssignTask(RangedAttackTask);
	
	// Wander
	WanderTask.Actions.Add(&MoveTimedAction);
	HtnDomain->AssignTask(WanderTask);
}

void ANpcFriendly::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterName.IsEmpty())
	{
		const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
		if (!GameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("ANpcFriendly::ANpcFriendly - Failed to get the game mode!"))		
			return;
		}
		CharacterName = GameMode->GetRandomNpcName();
	}
	
	if (NpcManager) NpcManager->AddNpc(this, ENpcTag::Friendly);
}

void ANpcFriendly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NpcManager) NpcManager->RemoveNpc(this, ENpcTag::Friendly);

	Super::EndPlay(EndPlayReason);
}