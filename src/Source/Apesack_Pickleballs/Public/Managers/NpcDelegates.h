
#pragma once

#include "CoreMinimal.h"

class AWall;
class ABuildingBase;

struct FNpcDelegates
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFurthestLeftWallChanged, AWall*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFurthestRightWallChanged, AWall*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewBuilding, ABuildingBase*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingDamaged, ABuildingBase*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingRepaired, ABuildingBase*);

	// called when a wall is built and its the furthest left of the origin
	static FOnFurthestLeftWallChanged OnFurthestLeftWallChanged;
	
	// called when a wall is built and its the furthest right of the origin 
	static FOnFurthestRightWallChanged OnFurthestRightWallChanged;

	// called when a new building is spawned in by the player
	static FOnNewBuilding OnNewBuilding;
	
	// Called when a building takes damage
	static FOnBuildingDamaged OnBuildingDamaged;
	
	// Called when a building goes back to max hp
	static FOnBuildingRepaired OnBuildingRepaired;
};