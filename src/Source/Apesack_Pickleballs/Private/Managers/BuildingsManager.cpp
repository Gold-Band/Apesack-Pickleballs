#include "Managers/BuildingsManager.h"
#include "Buildings/Building.h"

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

void UBuildingsManager::AddBuilding(ABuilding* NewBuilding, EBuildingType BuildingType)
{
	TArray<ABuilding*>* ModifyArray;
	
	if (BuildingType == EBuildingType::Wall)
	{
		ModifyArray = &AllWalls;
	}
	else
	{
		ModifyArray = &AllTowers;
	}
	
	bool bInserted = false;
	// insert in array based on distance
	for (int i = 0; i < ModifyArray->Num(); ++i)
	{
		if (NewBuilding->DistanceFromOrigin < (*ModifyArray)[i]->DistanceFromOrigin)
		{
			ModifyArray->Insert(NewBuilding, i);
			bInserted = true;
			break;
		}
	}
	if (!bInserted) ModifyArray->Add(NewBuilding);
	
	const FString Type = BuildingType == EBuildingType::Wall? "Walls" : "Towers";
	UE_LOG(LogTemp, Warning, TEXT("Num%s = %i  |  Leftmost = %s  |  Rightmost = %s"), *Type, ModifyArray->Num(), *(*ModifyArray)[0]->GetActorLabel(), *ModifyArray->Last()->GetActorLabel())
}

void UBuildingsManager::RemoveBuilding(ABuilding* OldBuilding, EBuildingType BuildingType)
{
	TArray<ABuilding*>* ModifyArray;
	
	if (BuildingType == EBuildingType::Wall)
	{
		ModifyArray = &AllWalls;
	}
	else
	{
		ModifyArray = &AllTowers;
	}
	
	ModifyArray->Remove(OldBuilding);
	//const int WallIndex = ModifyArray->Find(OldBuilding);
	//ModifyArray->RemoveAt(WallIndex);
}

AActor* UBuildingsManager::GetFarthestBuilding(EBuildingType Type, EOriginSide Side)
{
	TArray<ABuilding*>* SearchArray;
	
	if (Type == EBuildingType::Wall)
	{
		SearchArray = &AllWalls;
	}
	else
	{
		SearchArray = &AllTowers;
	}
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// for now
	return Cast<AActor>((*SearchArray)[0]);
// TODO(adam): unreachable
#if 0
	ABuilding* Result = nullptr;
	
	
	
	// test to make sure they are on the correct side
	if (Side == EOriginSide::Right)
	{
		Result = SearchArray->Last();
		if (Result->DistanceFromOrigin > 0)
		{
			return Result;
		}
	}
	else
	{
		Result = (*SearchArray)[0];
		if (Result->DistanceFromOrigin < 0)
		{
			return Result;
		}
	}
		
	return nullptr;
#endif
}

bool UBuildingsManager::TowersExist() const
{
	return AllTowers.Num() > 0;
}

bool UBuildingsManager::WallsExist() const
{
	return AllWalls.Num() > 0;
}
