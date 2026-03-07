#include "Managers/BuildingsManager.h"

#include "Buildings/ArcherTower.h"
#include "Buildings/Building.h"
#include "Buildings/Wall.h"

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

void UBuildingsManager::AddBuilding(ABuilding* NewBuilding, EBuildingType BuildingType, EOriginSide Side)
{
	TArray<ABuilding*>* ModifyArray;
	
	if (BuildingType == EBuildingType::Wall)
	{
		if (OnNewWallBuiltDelegate.IsBound()) OnNewWallBuiltDelegate.Broadcast(Cast<AWall>(NewBuilding), Side); 
		if (Side == EOriginSide::Left) ModifyArray = &LeftWalls;
		else ModifyArray = &RightWalls;
	}
	else
	{
		if (OnNewArcherTowerBuiltDelegate.IsBound()) OnNewArcherTowerBuiltDelegate.Broadcast(Cast<AArcherTower>(NewBuilding), Side); 
		if (Side == EOriginSide::Left) ModifyArray = &LeftTowers;
		else ModifyArray = &RightTowers;
	}
	
	bool bInserted = false;
	// insert in array based on distance
	for (int i = 0; i < ModifyArray->Num(); ++i)
	{
		const bool InsertCond = Side == EOriginSide::Left? 
		NewBuilding->DistanceFromOrigin < (*ModifyArray)[i]->DistanceFromOrigin:
		NewBuilding->DistanceFromOrigin > (*ModifyArray)[i]->DistanceFromOrigin;
		
		if (InsertCond)
		{
			ModifyArray->Add(NewBuilding);
			bInserted = true;
			break;
		}
	}
	if (!bInserted) ModifyArray->Add(NewBuilding);
	
	
	// print	
	FString TypeString;
	TArray<ABuilding*>* LeftArray;
	TArray<ABuilding*>* RightArray;
	if (BuildingType == EBuildingType::Wall)
	{
		TypeString = "Walls";
		LeftArray = &LeftWalls;
		RightArray = &RightWalls;
	}
	else
	{
		TypeString = "Towers";
		LeftArray = &LeftTowers;
		RightArray = &RightTowers;
	}
	
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Num%s = %i  |  Leftmost = %s  |  Rightmost = %s"), 
		*TypeString, LeftArray->Num()+RightArray->Num(), 
		LeftArray->IsEmpty()? TEXT("Nothing") : *LeftArray->Last()->GetActorNameOrLabel(), 
		RightArray->IsEmpty()? TEXT("Nothing") : *RightArray->Last()->GetActorNameOrLabel());
#endif
}

void UBuildingsManager::RemoveBuilding(ABuilding* OldBuilding, EBuildingType BuildingType, EOriginSide Side)
{
	TArray<ABuilding*>* ModifyArray;
	
	if (BuildingType == EBuildingType::Wall)
	{
		if (Side == EOriginSide::Left) ModifyArray = &LeftWalls;
		else ModifyArray = &RightWalls;
	}
	else
	{
		if (Side == EOriginSide::Left) ModifyArray = &LeftTowers;
		else ModifyArray = &RightTowers;
	}
	
	ModifyArray->Remove(OldBuilding);
}

AActor* UBuildingsManager::GetFarthestBuilding(EBuildingType Type, EOriginSide Side)
{
	TArray<ABuilding*>* SearchArray;
	
	if (Type == EBuildingType::Wall)
	{
		if (Side == EOriginSide::Left) SearchArray = &LeftWalls;
		else SearchArray = &RightWalls;
	}
	else
	{
		if (Side == EOriginSide::Left) SearchArray = &LeftTowers;
		else SearchArray = &RightTowers;
	}
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	return SearchArray->Last();
}

AActor* UBuildingsManager::GetNearestBuilding(const FVector& FromLocation, EBuildingType Type, EOriginSide Side, bool bDamaged)
{
	TArray<ABuilding*>* SearchArray;
	
	if (Type == EBuildingType::Wall)
	{
		if (Side == EOriginSide::Left) SearchArray = &LeftWalls;
		else SearchArray = &RightWalls;
	}
	else
	{
		if (Side == EOriginSide::Left) SearchArray = &LeftTowers;
		else SearchArray = &RightTowers;
	}
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// get closest
	AActor* FoundActor = nullptr;
	float ShortestDistance = UE_MAX_FLT;
	for (auto Building: *SearchArray)
	{
		const float FastDist = FVector::DistSquared(FromLocation, Building->GetActorLocation());
		if (FastDist < ShortestDistance && (!bDamaged || (bDamaged && Building->IsDamaged())))
		{
			ShortestDistance = FastDist;
			FoundActor = Building;
		}
	}
	
	return FoundActor;
	
}

bool UBuildingsManager::DoVacantTowersExist(EOriginSide Side) const
{
	const TArray<ABuilding*>* SearchArray;
	
	if (Side == EOriginSide::Left) SearchArray = &LeftTowers;
	else SearchArray = &RightTowers;
	
	for (const auto Tower: *SearchArray)
	{
		if (Cast<AArcherTower>(Tower)->HasRoom()) return true;
	}
	
	return false;
}

bool UBuildingsManager::WallsExist(EOriginSide Side) const
{
	if (Side == EOriginSide::Left) return !LeftWalls.IsEmpty();
	return !RightWalls.IsEmpty();
}
