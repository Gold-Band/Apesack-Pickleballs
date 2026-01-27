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

void APlot::SpawnBuilding(int IndexOfBuilding) const 
{
	//hide sprite
	SpriteComp->SetVisibility(false);
	
	// spawn building actor
	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	BuildingActor = GetWorld()->SpawnActor(CompatibleBuildings[IndexOfBuilding], &Location, &Rotation);
}

void APlot::OnClicked()
{
	OnActorClicked();
}

FString APlot::GetActorName() const
{
	return FString("Plot of Land");
}

TArray<UListItemObject*> APlot::GetActions()
{
	TArray<UListItemObject*> Actions{};
	
	int i = 0;
	for (auto Option : CompatibleBuildings)
	{
		if (!Option) continue;
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(Option.GetDefaultObject()->GetActorName());
		Action->Cost = Option.GetDefaultObject()->GetBuildCost();
		Action->ContextActor = this;

		const TFunction<void()> Func = [&, i](){SpawnBuilding(i);};
		Action->OnActionCalledFunction = Func;

		Actions.Add(Action);
		i++;
	}
	
	return Actions;
}
