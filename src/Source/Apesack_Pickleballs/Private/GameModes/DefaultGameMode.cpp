// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DefaultGameMode.h"

#include "Buildings/NpcShop.h"
#include "NPC/NpcName.h"
#include "Buildings/Plot.h"
#include "Buildings/Wall.h"
#include "LevelInstance/LevelInstanceTypes.h"
#include "Managers/BuildingsManager.h"
#include "Managers/NpcDelegates.h"
#include "Managers/NpcManager.h"
#include "NPC/NpcBase.h"
#include "WorldClock/WorldClockSubsystem.h"


bool ADefaultGameMode::GetBuilderShopLocation(FVector& OutLocation) const
{
	if (bBuildersUnlocked && !BuilderShop.IsStale())
	{
		OutLocation = BuilderShop.Get()->GetActorLocation();
		return true;
	}
	return false;
}

bool ADefaultGameMode::GetArcherShopLocation(FVector& OutLocation) const
{
	if (bBuildersUnlocked && !ArcherShop.IsStale())
	{
		OutLocation = ArcherShop.Get()->GetActorLocation();
		return true;
	}
	return false;
}

bool ADefaultGameMode::GetBlacksmithShopLocation(FVector& OutLocation) const
{
	if (bBuildersUnlocked && !BlacksmithShop.IsStale())
	{
		OutLocation = BlacksmithShop.Get()->GetActorLocation();
		return true;
	}
	return false;
}

FString ADefaultGameMode::GetRandomNpcName() const
{
	TArray<FNpcName*> AllNames;
	NpcNames.LoadSynchronous()->GetAllRows(TEXT("GetRandomName"), AllNames);
	return *AllNames[FMath::RandRange(0, AllNames.Num() - 1)]->SampleName;
}

TArray<FClassInfo*> ADefaultGameMode::GetAllClasses() const
{
	TArray<FClassInfo*> All;
	NpcClasses.LoadSynchronous()->GetAllRows<FClassInfo>(TEXT("GetAllClasses"),All);
	return All;
}

TArray<FBuildingInfo*> ADefaultGameMode::GetAllBuildings() const
{
	TArray<FBuildingInfo*> All;
	Buildings.LoadSynchronous()->GetAllRows<FBuildingInfo>(TEXT("GetAllClasses"),All);
	return All;
}

void ADefaultGameMode::RegisterShop(const ANpcShop* Shop, const EShopType ShopType)
{
	switch (ShopType)
	{
	case Builder:
		bBuildersUnlocked = true;
		BuilderShop = Shop;
		break;
	case Blacksmith:
		bSoldiersUnlocked = true;
		BlacksmithShop = Shop;
		break;
	case Archer:
		bArchersUnlocked = true;
		ArcherShop = Shop;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("RegisterShop - Unhandled shop type!"))
		break;
	}
}

void ADefaultGameMode::NewBuilding(ABuildingBase* Building, const EBuildingType BuildingType)
{
	if (!BuildingsManager) InitializeLocalBuildingsManagerReference();
	
	// where is this building?
	const float BuildingDistance = GetAngleBetweenVectors(Building->GetActorLocation(), WorldOriginNormal);
	Building->DistanceFromOrigin = BuildingDistance;
	
	// notify npcs
	switch (BuildingType)
	{
	case Wall:
		// is it the furthest wall?
		BuildingsManager->AddWall(Cast<AWall>(Building));
		if (FNpcDelegates::OnNewBuilding.IsBound()) FNpcDelegates::OnNewBuilding.Broadcast(Building);
		break;
	case ArcherTower:
		break;
	case Shop:
		break;
	default: 
		UE_LOG(LogTemp, Warning, TEXT("NewBuilding - Unhandled building type!"));
		break;
	}
	
}

void ADefaultGameMode::BuildingDestroyed(ABuildingBase* Building, const EBuildingType BuildingType)
{
	// notify npcs
	switch (BuildingType)
	{
	case Wall:
		// is it the furthest wall?
		BuildingsManager->RemoveWall(Cast<AWall>(Building));
		break;
	case ArcherTower:
		break;
	case Shop:
		break;
	default: 
		UE_LOG(LogTemp, Warning, TEXT("NewBuilding - Unhandled building type!"));
		break;
	}
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
	
	if (!BuildingsManager) InitializeLocalBuildingsManagerReference();
	if (!NpcManager) InitializeLocalNpcManagerReference();

	// setup an arrow pool
	if (ArrowClass)	ArrowPool.Initialize(GetWorld(), ArrowClass, 10);
}

void ADefaultGameMode::InitializeLocalBuildingsManagerReference()
{
	BuildingsManager = UBuildingsManager::Get(this);
	BuildingsManager->SetWorldOrigin(WorldOriginNormal);
}

void ADefaultGameMode::InitializeLocalNpcManagerReference()
{
	NpcManager = UNpcManager::Get(this);
	NpcManager->SetWorldOrigin(WorldOriginNormal);
}
