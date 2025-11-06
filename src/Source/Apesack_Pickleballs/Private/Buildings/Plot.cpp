#include "Buildings/Plot.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"

APlot::APlot()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(RootComponent);

	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollider);
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
