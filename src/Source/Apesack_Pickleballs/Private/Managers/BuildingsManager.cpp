#include "Managers/BuildingsManager.h"
#include "Buildings/BuildingBase.h"
#include "Buildings/Wall.h"
#include "Managers/NpcDelegates.h"

UBuildingsManager::UBuildingsManager()
{
	
}

UBuildingsManager* UBuildingsManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UBuildingsManager>();
		}
	}
	return nullptr;
}

void UBuildingsManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// cache all buildings
	
}

void UBuildingsManager::AddWall(AWall* Wall)
{
	// insert in array based on distance

	bool bInsterted = false;
	for (int i = 0; i < AllWalls.Num(); ++i)
	{
		if (Wall->DistanceFromOrigin < AllWalls[i]->DistanceFromOrigin)
		{
			AllWalls.Insert(Wall, i);
			bInsterted = true;
			break;
		}
	}
	if (!bInsterted) AllWalls.Add(Wall);
	
	if (FNpcDelegates::OnFurthestLeftWallChanged.IsBound()) FNpcDelegates::OnFurthestLeftWallChanged.Broadcast(AllWalls[0]);
	if (FNpcDelegates::OnFurthestRightWallChanged.IsBound())FNpcDelegates::OnFurthestRightWallChanged.Broadcast(AllWalls.Last());
	//UE_LOG(LogTemp, Warning, TEXT("NumWalls = %i  |  Leftmost = %s  |  Rightmost = %s"), AllWalls.Num(), *AllWalls[0]->GetActorLabel(), *AllWalls.Last()->GetActorLabel())
}

void UBuildingsManager::AddTower(ABuildingBase* Tower)
{
}

void UBuildingsManager::AddShop(ABuildingBase* Shop)
{
}

void UBuildingsManager::SetWorldOrigin(const FVector& NewWorldOrigin)
{
	WorldOrigin = NewWorldOrigin;
}
