#include "Buildings/Building.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/BuildingsManager.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Root);
	RootComponent->Mobility = EComponentMobility::Static;
}

void ABuilding::OnClicked()
{
	OnActorClicked();
}

FString ABuilding::GetActorName() const
{
	return Name;
}


int ABuilding::GetBuildCost() const
{
	return BuildCost;
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
	DistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(GetActorLocation());
	UBuildingsManager::Get(GetWorld())->AddBuilding(this, BuildingType);
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBuildingsManager::Get(GetWorld())->RemoveBuilding(this, BuildingType);
	Super::EndPlay(EndPlayReason);
}
