#include "Buildings/Building.h"

#include "StatsComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/BuildingsManager.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(Root);
	RootComponent->Mobility = EComponentMobility::Static;
	
	static ConstructorHelpers::FClassFinder<AActor> PlotClassFinder{TEXT("/Game/Blueprints/Buildings/BP_Plot")};
	if (PlotClassFinder.Succeeded())
	{
		PlotClass = PlotClassFinder.Class;
	}
}

void ABuilding::OnClicked()
{
	OnActorClicked();
}

TArray<UListItemObject*> ABuilding::GetActions()
{
	TArray<UListItemObject*> Actions;
	
	// undo action - destroy building
	UListItemObject* Action = NewObject<UListItemObject>();
	Action->DisplayText = FText::FromString(TEXT("Destroy"));
	Action->ContextActor = this;
	const TFunction<void()> Func = [&]()
	{
		// make this a plot again
		Destroy();
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();
		GetWorld()->SpawnActor(PlotClass, &Location, &Rotation);
	};
	Action->OnActionCalledFunction = Func;
	Action->Cost = 0;
	Action->bCloseOnClicked = true;
	Actions.Add(Action);
	
	return Actions;
}

FString ABuilding::GetActorName() const
{
	return Name;
}


int ABuilding::GetBuildCost() const
{
	return BuildCost;
}

bool ABuilding::IsDamaged() const
{
	if (!Stats) return false;
	return Stats->GetHealth() < Stats->GetMaxHealth();
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
	DistanceFromOrigin = ADefaultGameMode::GetAngleToOrigin(GetActorLocation());
	BuildingSide = DistanceFromOrigin < 0? EOriginSide::Left : EOriginSide::Right; 
	
	UBuildingsManager::Get(GetWorld())->AddBuilding(this, BuildingType, BuildingSide);
	
	Stats = Cast<UStatsComponent>(GetComponentByClass<UStatsComponent>());
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UBuildingsManager::Get(GetWorld())->RemoveBuilding(this, BuildingType, BuildingSide);
	Super::EndPlay(EndPlayReason);
}
