// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcCultist.h"

#include "AI/HTN/HTNComponent.h"
#include "Buildings/RitualZone.h"
#include "Buildings/Wall.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/NpcManager.h"
#include "Movement/CircularPawnMovementComponent.h"
#include "WorldClock/WorldClockSubsystem.h"

// Sets default values
ANpcCultist::ANpcCultist()
{
	NpcType = ENpcTag::Hostile;
	CharacterName = "Cultist";
}

void ANpcCultist::BeginPlay()
{
	MainSide = ADefaultGameMode::GetActorSideFromOrigin(this);
	
	Super::BeginPlay();
	WorldClockSubsystem = UWorldClockSubsystem::Get(GetWorld());
	
	BuildingsManager = UBuildingsManager::Get(GetWorld());
}

void ANpcCultist::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MySpawner && bIsOccupyingRitualZone) MySpawner->RemoveOccupant();
	BuildingsManager->OnNewWallBuiltDelegate.RemoveAll(this);
	BuildingsManager->OnWallDestroyedDelegate.RemoveAll(this);
	
	Super::EndPlay(EndPlayReason);
}

bool ANpcCultist::GetSideCheckCondition()
{
	return false;
}


void ANpcCultist::CreateBehaviours()
{
	FAction SummoningAction{"Summon Enemies"};
	SummoningAction.Func = [&](const float DeltaTime){return SummonEnemies(DeltaTime);};
	
	FAction MoveToVectorAction{FString("Move To Vector")};
	MoveToVectorAction.Func = [&](const float DeltaTime){ return MoveToVector(DeltaTime);};
	
	FAction SelectRitualZoneAction{FString("Select Ritual Zone")};
	SelectRitualZoneAction.Func = [&](const float DeltaTime){ return SelectRitualZone(DeltaTime);};
	
	FAction JoinRitualCircleAction{FString("Join Circle")};
	JoinRitualCircleAction.Func = [&](const float DeltaTime){return JoinRitualCircle(DeltaTime);};
	
	SummoningTask.Actions.Add(SummoningAction);
	SummoningTask.Condition = [&]{return SummonCondition();};
	SummoningTask.Cooldown = Cooldown_Summoning;
	
	OccupyRitualZoneTask.Actions.Add(SelectRitualZoneAction);
	OccupyRitualZoneTask.Actions.Add(MoveToVectorAction);
	OccupyRitualZoneTask.Actions.Add(JoinRitualCircleAction);
	OccupyRitualZoneTask.Condition = [&]{return OccupyRitualZoneCondition();};
	OccupyRitualZoneTask.Cooldown = 1.0f;
	
	HtnDomain->AssignTask(&SummoningTask);
	HtnDomain->AssignTask(&OccupyRitualZoneTask);
	
	// Wait
	Super::CreateBehaviours();
}

EActionState ANpcCultist::MoveToVector(float DeltaTime)
{
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetLocation);
	if (DistanceSquared <= StopDistance)
	{
		return EActionState::Succeeded;
	}
	
	// Move
	MoveDirection = GetDirectionTo(TargetLocation);
	MoveForwardScaled(MoveDirection);
	
	return EActionState::InProgress;
}

EActionState ANpcCultist::SelectRitualZone(float DeltaTime)
{
	MySpawner = UBuildingsManager::Get(GetWorld())->GetGotoRitualZone(MainSide);
	
	if (!MySpawner) return EActionState::Failed;
	
	const float Radius = MovementComp->Radius;
	TargetLocation = MySpawner->GetActorLocation().GetClampedToSize2D(Radius, Radius);
	
	return EActionState::Succeeded;
}

EActionState ANpcCultist::JoinRitualCircle(float DeltaTime)
{
	if (!MySpawner->HasRoom()) return EActionState::Failed;
	
	bIsOccupyingRitualZone = true;
	MySpawner->AddOccupant(this);
	OnArrivedAtRitualZoneEvent();
	
	BuildingsManager->OnNewWallBuiltDelegate.AddLambda([&](const AWall* Wall, const EOriginSide Side){RecalculateRitualStartTime();});
	BuildingsManager->OnWallDestroyedDelegate.AddLambda([&](const AWall* Wall, const EOriginSide Side){RecalculateRitualStartTime();});
	RecalculateRitualStartTime();
	
	return EActionState::Succeeded;
}

bool ANpcCultist::OccupyRitualZoneCondition() const
{
	return bCanMove && !bIsOccupyingRitualZone;
}

void ANpcCultist::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();
	
	// signal that more cultists are needed
	NpcManager->OnCultistDied(MainSide);
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
	return bSummonEnabled && (IsRitualTime() || bSummonEnemies) && bIsOccupyingRitualZone;
}

bool ANpcCultist::IsRitualTime()
{
	const bool bIsRitualTime = WorldClockSubsystem->GetHours() == RitualStartHour;
	if (!bRitualStarted && bIsRitualTime)
	{
		bRitualStarted = true;
		// on ritual started
		RitualQty = RitualBaseQty + WorldClockSubsystem->GetDays();	
	}
	
	return bIsRitualTime && RitualQty > 0;
}

void ANpcCultist::RecalculateRitualStartTime()
{
	// calculate when i should start spawning
	const AActor* FarthestWall = UBuildingsManager::Get(GetWorld())->GetFarthestBuilding(EBuildingType::Wall,MainSide);
	const float FarthestWallAngle = FarthestWall? Cast<AWall>(FarthestWall)->DistanceFromOrigin : 0;
	const float AngleToOrigin = ADefaultGameMode::GetAngleToOrigin(GetActorLocation());
	const float AngleToWall = FMath::Abs(AngleToOrigin) - FMath::Abs(FarthestWallAngle);
	
	
	//At timescale=140 and movespeed=200, gremlins will travel ~15.3 degrees per hour 
	constexpr float AngularSpeed = 15.3f;
	
	RitualStartHour = WorldClockSubsystem->GetNightStartHour() - AngleToWall/AngularSpeed + 1;
	
	//UE_LOG(LogTemp, Warning, TEXT("Spawn hour = %i"), RitualStartHour)
}
