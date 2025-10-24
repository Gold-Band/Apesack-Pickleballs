#include "Buildings/Plot.h"

APlot::APlot()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlot::SetBuilding(const TSubclassOf<AActor> BuildingActorClass, const FBuildingInfo* BuildingInfo)
{
	if (!BuildingActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildingInfo is null!"))
		return;
	}
	if (BuildingActor) BuildingActor->Destroy();
	BuildingActor = GetWorld()->SpawnActor(BuildingActorClass, &GetTransform());
	Building = BuildingInfo;
	
}

void APlot::BeginPlay()
{
	Super::BeginPlay();
}
