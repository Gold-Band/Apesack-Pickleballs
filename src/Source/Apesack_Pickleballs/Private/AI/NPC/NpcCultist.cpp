// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcCultist.h"

#include "AI/HTN/HTNComponent.h"
#include "Managers/NpcManager.h"
#include "WorldClock/WorldClockSubsystem.h"

// Sets default values
ANpcCultist::ANpcCultist()
{
	NpcType = ENpcTag::Hostile;
	CharacterName = "Cultist";
	
	GetSideInterval = 1;
}

void ANpcCultist::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if ((SenseTimer+=DeltaSeconds) > SenseFriendliesInterval)
	{
		SenseTimer = 0;
		TargetActor = NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyFriendly, MainSide, DetectDangerRadius);
	}
}

void ANpcCultist::BeginPlay()
{
	Super::BeginPlay();
	WorldClockSubsystem = UWorldClockSubsystem::Get(this);
}

void ANpcCultist::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ANpcCultist::BindActions()
{
	RunawayAction.ExecutionDelegate.BindUObject(this, &ANpcCultist::Runaway);
	RunawayAction.ConditionDelegate.BindUObject(this, &ANpcCultist::RunawayCondition);
	
	SummoningAction.ExecutionDelegate.BindUObject(this, &ANpcCultist::SummonEnemies);
	SummoningAction.ConditionDelegate.BindUObject(this, &ANpcCultist::SummonCondition);
}

void ANpcCultist::CreateBehaviours()
{
	RunawayTask.Actions.Add(&RunawayAction);
	HtnDomain->AssignTask(&RunawayTask);
	
	SummoningTask.Actions.Add(&SummoningAction);
	HtnDomain->AssignTask(&SummoningTask);
}

void ANpcCultist::Runaway(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Runaway"));
}

bool ANpcCultist::RunawayCondition() const
{
	return TargetActor != nullptr;
}

void ANpcCultist::SummonEnemies(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Summon"));
}

bool ANpcCultist::SummonCondition()
{
	const float DistSquared = TargetActor != nullptr? FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation()) : UE_MAX_FLT;
	return WorldClockSubsystem->GetHours() > RitualStartHour || DistSquared < FMath::Square(RunawayRadius);
}
