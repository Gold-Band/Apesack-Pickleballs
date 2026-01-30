
#pragma once

#include "CoreMinimal.h"
#include "BuildingsManager.generated.h"

class ABuilding;
class AArcherTower;
class AWall;

UENUM(BlueprintType, Blueprintable)
enum class EBuildingType : uint8
{
	Wall,
	Tower
};

UENUM(BlueprintType, Blueprintable)
enum class EOriginSide : uint8
{
	Left,
	Right
};

UCLASS()
class UBuildingsManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UBuildingsManager();
	
	static UBuildingsManager* Get(const UObject* WorldContextObject);

	void AddBuilding(ABuilding* NewBuilding, EBuildingType BuildingType);
	void RemoveBuilding(ABuilding* OldBuilding, EBuildingType BuildingType);
	
	AActor* GetFarthestBuilding(EBuildingType Type, EOriginSide Side);
	
	bool DoVacantTowersExist() const;
	bool WallsExist() const;
	
private:
	
	UPROPERTY()
	TArray<ABuilding*> AllWalls;
	
	UPROPERTY()
	TArray<ABuilding*> AllTowers;
};
