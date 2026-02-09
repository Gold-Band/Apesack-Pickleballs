
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
	Right,
	Any
};


// delegate for when an archer tower is built
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNewArcherTowerBuiltSignature, AArcherTower*, EOriginSide);

// delegate for when a wall is built
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNewWallBuiltSignature, AWall*, EOriginSide);



UCLASS()
class UBuildingsManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UBuildingsManager();
	
	static FOnNewArcherTowerBuiltSignature OnNewArcherTowerBuiltDelegate;
	static FOnNewWallBuiltSignature OnNewWallBuiltDelegate;
	
	static UBuildingsManager* Get(const UObject* WorldContextObject);

	void AddBuilding(ABuilding* NewBuilding, EBuildingType BuildingType, EOriginSide Side);
	void RemoveBuilding(ABuilding* OldBuilding, EBuildingType BuildingType, EOriginSide Side);
	
	AActor* GetFarthestBuilding(EBuildingType Type, EOriginSide Side);
	
	bool DoVacantTowersExist(EOriginSide Side) const;
	bool WallsExist(EOriginSide Side) const;
	
private:
	
	UPROPERTY()
	TArray<ABuilding*> RightWalls;
	
	UPROPERTY()
	TArray<ABuilding*> LeftWalls;
	
	UPROPERTY()
	TArray<ABuilding*> RightTowers;
	
	UPROPERTY()
	TArray<ABuilding*> LeftTowers;
};
