
#pragma once

#include "CoreMinimal.h"

class ABuildingBase;

struct FNpcDelegates
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFurthestLeftWallChanged, ABuildingBase*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFurthestRightWallChanged, ABuildingBase*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewBuilding, ABuildingBase*);

	// called when a wall is built and its the furthest left of the origin 
	static FOnFurthestLeftWallChanged OnFurthestLeftWallChanged;
	
	// called when a wall is built and its the furthest right of the origin 
	static FOnFurthestRightWallChanged OnFurthestRightWallChanged;

	// called when a new building is spawned in by the player
	static FOnNewBuilding OnNewBuilding;
};