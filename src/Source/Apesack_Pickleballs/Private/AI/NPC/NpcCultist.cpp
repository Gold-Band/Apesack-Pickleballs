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
	
	Super::BindActions();
}

void ANpcCultist::CreateBehaviours()
{
	RunawayTask.Actions.Add(&RunawayAction);
	HtnDomain->AssignTask(&RunawayTask);
	
	SummoningTask.Actions.Add(&SummoningAction);
	HtnDomain->AssignTask(&SummoningTask);
	
	Super::CreateBehaviours();
}

void ANpcCultist::Runaway(float DeltaTime)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Runaway"));
#endif
	
	RunawayAction.State = EActionState::Succeeded;
}

bool ANpcCultist::RunawayCondition() const
{
	return TargetActor != nullptr;
}

void ANpcCultist::SummonEnemies(float DeltaTime)
{
	if (IsRitualTime() && RitualQty-- > 0)
	{
		SummonEnemiesEvent();
		SummoningAction.State = EActionState::Succeeded;
		return;
	}
	
	// otherwise, the player is probably invading
	SummonEnemiesEvent();
	SummoningAction.State = EActionState::Succeeded;
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
