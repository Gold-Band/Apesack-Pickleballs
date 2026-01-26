#include "Buildings/Plot.h"
#include "PaperSpriteComponent.h"
#include "AI/HTN/ListItemObject.h"
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

void APlot::OnClicked()
{
	OnActorClicked();
}

FString APlot::GetActorName() const
{
	return FString("Plot of Land");
}

TArray<UListItemObject*> APlot::GetActions() const
{
	TArray<UListItemObject*> Actions{};
	
	for (auto Option : CompatibleBuildings)
	{
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(Option.GetDefaultObject()->GetActorName());
		Action->Cost = Option.GetDefaultObject()->GetBuildCost();
		Action->ContextActor = this;
		Action->OnActionCalledFunction = &ThisClass::TestFunction;
		//auto test =  &ThisClass::TestFunction;
		Actions.Add(Action);
	}
	
	return Actions;
}
