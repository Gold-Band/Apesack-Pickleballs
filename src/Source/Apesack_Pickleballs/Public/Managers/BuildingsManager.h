
#pragma once

#include "CoreMinimal.h"
#include "BuildingsManager.generated.h"

class ABuilding;
class ARitualZone;
class AArcherTower;
class AWall;

UENUM(BlueprintType, Blueprintable)
enum class EBuildingType : uint8
{
	Wall,
	Tower,
	Ritual,
	Any
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

// delegate for when a wall is built
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWallDestroyedSignature, AWall*, EOriginSide);


UCLASS()
class UBuildingsManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UBuildingsManager(){};
	
	FOnNewArcherTowerBuiltSignature OnNewArcherTowerBuiltDelegate;
	FOnNewWallBuiltSignature OnNewWallBuiltDelegate;
	FOnWallDestroyedSignature OnWallDestroyedDelegate;
	
	static UBuildingsManager* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void AddBuilding(ABuilding* NewBuilding, EBuildingType BuildingType, EOriginSide Side);
	UFUNCTION(BlueprintCallable)
	void RemoveBuilding(ABuilding* OldBuilding, const EBuildingType BuildingType, const EOriginSide Side);
	
	AActor* GetFarthestBuilding(EBuildingType Type, EOriginSide Side);
	AActor* GetNearestBuilding(const FVector& FromLocation, EBuildingType Type, EOriginSide Side, bool bDamaged);
	ARitualZone* GetGotoRitualZone(EOriginSide Side);
	bool DoVacantTowersExist(EOriginSide Side) const;
	bool WallsExist(EOriginSide Side) const;
	
private:
	const TArray<ABuilding*>* GetArrayConst(EBuildingType Type, EOriginSide Side) const;
	TArray<ABuilding*>* GetArray(EBuildingType Type, EOriginSide Side);
	
	UPROPERTY()
	TArray<ABuilding*> RightWalls;
	
	UPROPERTY()
	TArray<ABuilding*> LeftWalls;
	
	UPROPERTY()
	TArray<ABuilding*> RightTowers;
	
	UPROPERTY()
	TArray<ABuilding*> LeftTowers;
	
	UPROPERTY()
	TArray<ABuilding*> RightRitualZones;
	
	UPROPERTY()
	TArray<ABuilding*> LeftRitualZones;
};
