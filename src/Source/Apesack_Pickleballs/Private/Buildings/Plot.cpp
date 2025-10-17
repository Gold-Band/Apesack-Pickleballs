#include "Buildings/Plot.h"

APlot::APlot()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlot::SetBuilding(const TSubclassOf<AActor> BuildingActorClass, const FDataTableRowHandle& RowHandle)
{
	if (!BuildingActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildingInfo is null!"))
		return;
	}
	BuildingActor = GetWorld()->SpawnActor(BuildingActorClass);
	Building = RowHandle;
}

void APlot::BeginPlay()
{
	Super::BeginPlay();
}
