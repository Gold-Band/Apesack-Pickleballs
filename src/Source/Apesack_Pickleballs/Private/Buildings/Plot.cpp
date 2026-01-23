#include "Buildings/Plot.h"
#include "PaperSpriteComponent.h"
#include "Buildings/Building.h"
#include "Components/BoxComponent.h"

APlot::APlot()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(RootComponent);

	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollider);
}

void APlot::SpawnBuilding(int IndexOfBuilding)
{
	//hide sprite
	SpriteComp->SetVisibility(false);
	
	// spawn building actor
	UClass* Class = CompatibleBuildings[IndexOfBuilding]->StaticClass();
	BuildingActor = Cast<ABuilding>(GetWorld()->SpawnActor(Class));
}
