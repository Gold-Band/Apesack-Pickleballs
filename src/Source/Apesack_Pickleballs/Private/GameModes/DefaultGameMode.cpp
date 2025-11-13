// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DefaultGameMode.h"

#include "Buildings/NpcShop.h"
#include "NPC/NpcName.h"
#include "Buildings/Plot.h"
#include "Managers/NpcDelegates.h"
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
		UE_LOG(LogTemp, Warning, TEXT("ANpcShop - Unhandled shop type!"))
		break;
	}
}

void ADefaultGameMode::NewBuilding(ABuildingBase* Building)
{
	if (!FNpcDelegates::OnNewBuilding.IsBound()) return;
	FNpcDelegates::OnNewBuilding.Broadcast(Building);
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

	if (ArrowClass)	ArrowPool.Initialize(GetWorld(), ArrowClass, 10);
}
