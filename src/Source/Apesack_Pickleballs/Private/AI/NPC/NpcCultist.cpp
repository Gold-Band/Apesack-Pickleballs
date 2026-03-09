// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcCultist.h"

#include "AI/HTN/HTNComponent.h"
#include "GameModes/DefaultGameMode.h"
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
		TargetActor = NpcManager->FindNearestNpcOrPlayer(GetActorLocation(), ENpcSearchOption::AnyFriendly, MainSide, FMath::Square(RunawayRadius));
	}
}

void ANpcCultist::BeginPlay()
{
	
	const float DistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(GetActorLocation());
	MainSide = DistanceFromOrigin < 0? EOriginSide::Left : EOriginSide::Right;
	
	Super::BeginPlay();
	WorldClockSubsystem = UWorldClockSubsystem::Get(GetWorld());
}


void ANpcCultist::CreateBehaviours()
{
	
	FAction SummoningAction{"SummonEnemies"};
	SummoningAction.Func = [&](const float DeltaTime){return SummonEnemies(DeltaTime);};
	FAction RunawayAction{"Runaway"};
	RunawayAction.Func = [&](const float DeltaTime){return Runaway(DeltaTime);};
	
	RunawayTask.Actions.Add(RunawayAction);
	RunawayTask.Condition = [&]{return RunawayCondition();};
	SummoningTask.Actions.Add(SummoningAction);
	SummoningTask.Condition = [&]{return SummonCondition();};
	
	
	
	HtnDomain->AssignTask(&RunawayTask);
	HtnDomain->AssignTask(&SummoningTask);
	
	// Wait
	Super::CreateBehaviours();
}

EActionState ANpcCultist::Runaway(float DeltaTime)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Runaway"));
#endif
	
	return EActionState::Succeeded;
}

bool ANpcCultist::RunawayCondition() const
{
	return TargetActor != nullptr;
}

EActionState ANpcCultist::SummonEnemies(float DeltaTime)
{
	if (IsRitualTime() && RitualQty-- > 0)
	{
		SummonEnemiesEvent();
		return EActionState::Succeeded;
	}
	
	// otherwise, the player is probably invading
	SummonEnemiesEvent();
	return EActionState::Succeeded;
}

bool ANpcCultist::SummonCondition()
{
	return bSummonEnabled && (IsRitualTime() || bSummonEnemies);
}

bool ANpcCultist::IsRitualTime()
{
	const bool bIsRitualTime = (WorldClockSubsystem? WorldClockSubsystem->GetHours() >= RitualStartHour : false);
	if (!bRitualStarted && bIsRitualTime)
	{
		bRitualStarted = true;
		// on ritual started
		RitualQty = RitualBaseQty + WorldClockSubsystem->GetDays();	
	}
	
	return bIsRitualTime && RitualQty > 0;
}
