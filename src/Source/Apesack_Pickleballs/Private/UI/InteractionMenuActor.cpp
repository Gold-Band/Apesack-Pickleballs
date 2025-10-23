#include "UI/InteractionMenuActor.h"

#include "Buildings/Plot.h"
#include "Components/WidgetComponent.h"
#include "NPC/NpcCharacter.h"
#include "Managers/InteractionManager.h"
#include "UI/OptionsWidget.h"

AInteractionMenuActor::AInteractionMenuActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetVisibility(false);
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetCastShadow(false);
	WidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));

	WidgetComponent->SetRelativeRotation(FRotator(0, 90, 0));
	WidgetComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/NPC/UI/WBP_NpcOptions.WBP_NpcOptions_C"));
	if (WidgetFinder.Succeeded())
	{
		NpcInteractionWidgetClass = WidgetFinder.Class;
	}
}

void AInteractionMenuActor::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionsManager = UInteractionManager::Get(this);
}

void AInteractionMenuActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowActor)
	{
		SetActorLocation(FollowActor->GetActorLocation() + FollowOffset);
	}

}

void AInteractionMenuActor::SetInteractionContext(EInteractionContext Context)
{
	if (!NpcInteractionWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AInteractionMenuActor::SetInteractionContext - NpcInteractionWidgetClass is NULL!"));
		return;
	}
	
	switch (Context)
	{
	case EInteractionContext::NpcCharacter:
		if (NpcInteractionWidget)
		{
			WidgetComponent->SetWidget(NpcInteractionWidget);
		}
		else
		{
			WidgetComponent->SetWidgetClass(NpcInteractionWidgetClass);
			NpcInteractionWidget = Cast<UOptionsWidget>(WidgetComponent->GetWidget());
		}
		break;
	case EInteractionContext::Building:
		break;
	}
}

void AInteractionMenuActor::SetFollowActor(AActor* Actor)
{
	FollowActor = Actor;
}

template <>
bool AInteractionMenuActor::OpenInteractionDialog<ANpcCharacter>(ANpcCharacter* Actor)
{
	// Prepare the widget
	SetInteractionContext(EInteractionContext::NpcCharacter);
	if (!NpcInteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("AInteractionMenuActor::OpenInteractionDialog - Failed to setup widget!"));
		return false;
	}

	
	// Setup options widget to have the correct options based on the npc
	TArray<TOptionsData<FToolInfo>> OptionInitializers;

	const FClassInfo* ActorClass = Actor->GetClassInfo();
	const FToolInfo* NpcTool = Actor->GetTool();

	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Has No Class Assigned!"));
	}
	
	if (NpcTool && !NpcTool->NextTool.IsNull()) // if theres an upgrade available
	{
		const FToolInfo* Upgrade = NpcTool->NextTool.GetRow<FToolInfo>(TEXT("Get Upgrade"));
		check(Upgrade);
		OptionInitializers.Add(TOptionsData(Upgrade->ToolIcon, Upgrade->PurchaseCost, InteractionsManager->GetUpgradeTaskForTool(Upgrade), Upgrade));
	}

	// add all other base tools
	if (ActorClass && !InteractionsManager.IsExplicitlyNull())
	{
		for (const auto Class : InteractionsManager->GetAllClasses())
		{
			if (Class != ActorClass && !Class->BaseTool.IsNull())
			{
				const FToolInfo* Tool = Class->BaseTool.GetRow<FToolInfo>(TEXT("Get Base Tool"));
				check(Tool);
				OptionInitializers.Add(TOptionsData(Tool->ToolIcon, Tool->PurchaseCost, InteractionsManager->GetPromotionTaskForClass(Class), Tool));
			}
		}
	}
	
	if (OptionInitializers.Num() > 0)
	{
		NpcInteractionWidget->Setup<FToolInfo>(OptionInitializers);
		WidgetComponent->SetVisibility(true);
		return true;
	}
	// Add npc's name to this list so it looks good

	return false;
}

template <>
bool AInteractionMenuActor::OpenInteractionDialog<APlot>(APlot* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AInteractionMenuActor::OpenInteractionDialog - Plot"));
	// Prepare the widget
	SetInteractionContext(EInteractionContext::NpcCharacter);
	if (!NpcInteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("AInteractionMenuActor::OpenInteractionDialog - Failed to setup widget!"));
		return false;
	}

	// Setup options widget to have the correct options based on the plot
	TArray<TOptionsData<FBuildingInfo>> OptionInitializers;

	if (!InteractionsManager.IsExplicitlyNull())
	{
		if (!Actor->Building) // if is empty plot
		{
			for (const auto Building : InteractionsManager->GetAllBuildings())
			{
				if (Building->CompatiblePlotTags.HasTag(Actor->PlotTag) && Building->Level == 0)
				{
					OptionInitializers.Add(TOptionsData(Building->BuildingIcon, Building->PurchaseCost, nullptr, Building));
				}
			}
		}
		else
		{
			// get current building's upgrade
			check(Actor->Building);
			if (Actor->Building->NextBuilding.IsNull()) return false;

			const FBuildingInfo* Upgrade = Actor->Building->NextBuilding.GetRow<FBuildingInfo>(TEXT("Get Upgrade"));
			check(Upgrade);
			OptionInitializers.Add(TOptionsData(Upgrade->BuildingIcon, Upgrade->PurchaseCost, nullptr, Upgrade));
		}
	}
	
	if (OptionInitializers.Num() > 0)
	{
		NpcInteractionWidget->Setup(OptionInitializers);
		WidgetComponent->SetVisibility(true);
		return true;
	}

	return false;
}

void AInteractionMenuActor::CloseInteractionDialog()
{
	WidgetComponent->SetVisibility(false);
	if (NpcInteractionWidget) NpcInteractionWidget->Reset();
	
}

UGridNode* AInteractionMenuActor::GetMostRelevantNode()
{
	if (!NpcInteractionWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMostRelevantNode - NpcInteractionWidget is Null!"));
		return nullptr;
	}

	return NpcInteractionWidget->GetNode(0);
}


