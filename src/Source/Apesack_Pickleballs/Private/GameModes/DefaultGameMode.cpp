// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DefaultGameMode.h"

#include "AI/NPC/NpcName.h"
#include "WorldClock/WorldClockSubsystem.h"


FVector ADefaultGameMode::WorldOriginNormal = FVector(0.0f, 1.0f, 0.0f);

FString ADefaultGameMode::GetRandomNpcName() const
{
	TArray<FNpcName*> AllNames;
	NpcNames.LoadSynchronous()->GetAllRows(TEXT("GetRandomName"), AllNames);
	return *AllNames[FMath::RandRange(0, AllNames.Num() - 1)]->SampleName;
}

float ADefaultGameMode::GetAngleBetweenVectors(const FVector& A, const FVector& B)
{
	// Guard against zero‑length vectors
	if (A.IsNearlyZero() || B.IsNearlyZero())
	{
		return 0.f;
	}
	const float Dot = FVector::DotProduct(A, B);
	const float CrossDot = FVector::CrossProduct(A, B).Dot(FVector::UpVector);
	return FMath::RadiansToDegrees(FMath::Atan2(CrossDot, Dot));
}

float ADefaultGameMode::GetDistanceToOrigin(const FVector& FromLocation)
{
	return GetAngleBetweenVectors(FromLocation, WorldOriginNormal);
}

// eventually change this into GetProjectile(EProjectileType type)
AActor* ADefaultGameMode::GetArrow()
{
	return ArrowPool.GetActor();
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// cache managers
	WorldClock = UWorldClockSubsystem::Get(this);
	WorldClock->SetTime(GameStartTime.Day, GameStartTime.Hour, GameStartTime.Minute, GameStartTime.Second);
	WorldClock->SetTimeScale(GameTimeScale);
	WorldClock->AllowClockTicking(bEnableClock);
	
	// setup an arrow pool
	if (ArrowClass)	ArrowPool.Initialize(GetWorld(), ArrowClass, 10);
}