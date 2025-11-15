
#pragma once

#include "CoreMinimal.h"
#include "BuildingsManager.generated.h"


class AWall;
class ABuildingBase;

UCLASS()
class UBuildingsManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UBuildingsManager();
	
	static UBuildingsManager* Get(const UObject* WorldContextObject);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void AddWall(AWall* Wall);
	void AddTower(ABuildingBase* Tower);
	void AddShop(ABuildingBase* Shop);
	void SetWorldOrigin(const FVector& NewWorldOrigin);	
	
private:
	FVector WorldOrigin;
	
	TArray<AWall*> AllWalls;
	TArray<ABuildingBase*> AllTowers;
	TArray<ABuildingBase*> AllShops;
};
