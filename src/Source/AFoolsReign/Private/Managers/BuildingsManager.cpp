#include "Managers/BuildingsManager.h"

#include "Buildings/ArcherTower.h"
#include "Buildings/Building.h"
#include "Buildings/RitualZone.h"
#include "Buildings/Wall.h"

UBuildingsManager::UBuildingsManager()
{
	static ConstructorHelpers::FClassFinder<AActor> PlotClassFinder{TEXT("/Game/Blueprints/Buildings/BP_Plot")};
	if (PlotClassFinder.Succeeded())
	{
		PlotClass = PlotClassFinder.Class;
	}
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
	TArray<ABuilding*>* ModifyArray = GetArray(BuildingType,Side);
	if (!ModifyArray) return;
	
	if (BuildingType == EBuildingType::Wall)
	{
		if (OnNewWallBuiltDelegate.IsBound()) OnNewWallBuiltDelegate.Broadcast(Cast<AWall>(NewBuilding), Side); 
	}
	else if (BuildingType == EBuildingType::Tower)
	{
		if (OnNewArcherTowerBuiltDelegate.IsBound()) OnNewArcherTowerBuiltDelegate.Broadcast(Cast<AArcherTower>(NewBuilding), Side); 
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
#if WITH_EDITOR
	FString TypeString;
	TArray<ABuilding*>* LeftArray;
	TArray<ABuilding*>* RightArray;
	if (BuildingType == EBuildingType::Wall)
	{
		TypeString = "Walls";
		LeftArray = &LeftWalls;
		RightArray = &RightWalls;
	}
	else if (BuildingType == EBuildingType::Tower)
	{
		TypeString = "Towers";
		LeftArray = &LeftTowers;
		RightArray = &RightTowers;
	}
	else return;
	
	UE_LOG(LogTemp, Warning, TEXT("Num%s = %i  |  Leftmost = %s  |  Rightmost = %s"), 
		*TypeString, LeftArray->Num()+RightArray->Num(), 
		LeftArray->IsEmpty()? TEXT("Nothing") : *LeftArray->Last()->GetActorNameOrLabel(), 
		RightArray->IsEmpty()? TEXT("Nothing") : *RightArray->Last()->GetActorNameOrLabel());
#endif
}

void UBuildingsManager::RemoveBuilding(ABuilding* OldBuilding, const EBuildingType BuildingType, const EOriginSide Side)
{
	TArray<ABuilding*>* ModifyArray = GetArray(BuildingType,Side);
	
	if (!ModifyArray) return;
	
	ModifyArray->Remove(OldBuilding);
	
	if (BuildingType == EBuildingType::Wall && OnWallDestroyedDelegate.IsBound())
	{
		OnWallDestroyedDelegate.Broadcast(Cast<AWall>(OldBuilding), Side);
	}
}

AActor* UBuildingsManager::GetFarthestBuilding(EBuildingType Type, EOriginSide Side)
{
	TArray<ABuilding*>* SearchArray = GetArray(Type,Side);
	
	if (!SearchArray || SearchArray->IsEmpty()) return nullptr;
	
	return SearchArray->Last();
}

AActor* UBuildingsManager::GetNearestBuilding(const FVector& FromLocation, EBuildingType Type, EOriginSide Side, bool bDamaged)
{
	TArray<ABuilding*>* SearchArray = GetArray(Type,Side);
	
	if (!SearchArray || SearchArray->IsEmpty()) return nullptr;
	
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

ARitualZone* UBuildingsManager::GetGotoRitualZone(EOriginSide Side)
{
	const TArray<ABuilding*>* SearchArray = GetArray(EBuildingType::Ritual, Side);
	if (!SearchArray || SearchArray->IsEmpty()) return nullptr;
	
	// get the ritual zone with the least amount of cultists
	int Least = 99999;
	ARitualZone* ZoneWithLeastOccupants = nullptr;
	for (int i = 0; i < SearchArray->Num(); ++i)
	{
		const auto Zone = Cast<ARitualZone>((*SearchArray)[i]);
		const int NumOccupants = Zone->GetNumOccupants();
		if (NumOccupants < Least)
		{
			Least = NumOccupants;
			ZoneWithLeastOccupants = Zone;
		}
	}
	
	return ZoneWithLeastOccupants;
}

bool UBuildingsManager::DoVacantTowersExist(EOriginSide Side) const
{
	const TArray<ABuilding*>* SearchArray = GetArrayConst(EBuildingType::Tower, Side);
	
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

bool UBuildingsManager::DoVacantRitualZonesExist(const EOriginSide Side) const
{
	const TArray<ABuilding*>* SearchArray = GetArrayConst(EBuildingType::Ritual, Side);

	for (const auto Zone : *SearchArray)
	{
		if (Cast<ARitualZone>(Zone)->HasRoom()) return true;
	}
	
	return false;
}

const TArray<ABuilding*>* UBuildingsManager::GetArrayConst(EBuildingType Type, EOriginSide Side) const
{
	if (Type == EBuildingType::Wall)
	{
		if (Side == EOriginSide::Left) return &LeftWalls;
		return  &RightWalls;
	}
	if (Type == EBuildingType::Tower)
	{
		if (Side == EOriginSide::Left) return &LeftTowers;
		return &RightTowers;
	}
	if (Type == EBuildingType::Ritual)
	{
		if (Side == EOriginSide::Left) return &LeftRitualZones;
		return &RightRitualZones;
	}
	
	return nullptr;
}

TArray<ABuilding*>* UBuildingsManager::GetArray(EBuildingType Type, EOriginSide Side)
{
	if (Type == EBuildingType::Wall)
	{
		if (Side == EOriginSide::Left) return &LeftWalls;
		return  &RightWalls;
	}
	if (Type == EBuildingType::Tower)
	{
		if (Side == EOriginSide::Left) return &LeftTowers;
		return &RightTowers;
	}
	if (Type == EBuildingType::Ritual)
	{
		if (Side == EOriginSide::Left) return &LeftRitualZones;
		return &RightRitualZones;
	}
	
	return nullptr;
}
