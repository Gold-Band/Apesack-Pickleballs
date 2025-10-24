#include "Managers/InteractionManager.h"

#include "Buildings/Plot.h"
#include "NPC/NpcFriendly.h"
#include "UI/GridNode.h"
#include "HTN/Task.h"
#include "UI/InteractionMenuActor.h"

UInteractionManager::UInteractionManager()
{
	{
		ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/NPC/DT_Classes.DT_Classes"));
		if (DataTableFinder.Succeeded())
		{
			// populate AllClasses
			DataTableFinder.Object->GetAllRows<FClassInfo>(TEXT("Caching All Classes"), AllClasses);
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Tasks/TDA_WaitTask.TDA_WaitTask"));
		if (TaskFinder.Succeeded())
		{
			WaitTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Tasks/TDA_EmptyTask.TDA_EmptyTask"));
		if (TaskFinder.Succeeded())
		{
			EmptyTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Tasks/TDA_PromoteBuilder.TDA_PromoteBuilder"));
		if (TaskFinder.Succeeded())
		{
			PromoteBuilderTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Tasks/TDA_PromoteMelee.TDA_PromoteMelee"));
		if (TaskFinder.Succeeded())
		{
			PromoteMeleeTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Tasks/TDA_PromoteRanged.TDA_PromoteRanged"));
		if (TaskFinder.Succeeded())
		{
			PromoteRangedTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/NPC/DT_Buildings.DT_Buildings"));
		if (DataTableFinder.Succeeded())
		{
			// populate AllClasses
			DataTableFinder.Object->GetAllRows<FBuildingInfo>(TEXT("Caching All Buildings"), AllBuildings);
		}
	}
}

UInteractionManager* UInteractionManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UInteractionManager>();
		}
	}
	return nullptr;
}

void UInteractionManager::SetNpcClass(ANpcFriendly* NpcActor, FClassInfo* ClassInfo)
{
	
}

void UInteractionManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InteractionMenuActor = InWorld.SpawnActor<AInteractionMenuActor>(AInteractionMenuActor::StaticClass());
	if (!InteractionMenuActor) UE_LOG(LogTemp, Error, TEXT("UNpcManager::OnWorldBeginPlay -> Failed to spawn InteractionMenuActor!"));
}

void UInteractionManager::StartInteraction(AActor* Actor)
{

	if (bIsInteracting || !InteractionMenuActor || !Actor) return;
	bIsInteracting = true;

	//UE_LOG(LogTemp, Warning, TEXT("StartInteraction"));

	if (ANpcFriendly* NpcActor = Cast<ANpcFriendly>(Actor))
	{
		// Order npc to wait (DA_Wait)
		if (WaitTask.IsValid()) NpcActor->ForceTask(WaitTask);
		
		// // Try to open an interaction dialogue
		if (InteractionMenuActor->OpenInteractionDialog(NpcActor))
		{
			// Save ref
			CharacterWeAreInteractingWith = NpcActor;
		}

		// Hide npc's nametag..?
	}
	else if (APlot* PlotActor = Cast<APlot>(Actor))
	{
		// Try to open an interaction dialogue
		if (InteractionMenuActor->OpenInteractionDialog(PlotActor))
		{
			// Save ref
			PlotWeAreInteractingWith = PlotActor;
		}
	}

	// generic
	InteractionMenuActor->SetFollowActor(Actor);
	SelectedOptionNode = InteractionMenuActor->GetMostRelevantNode();
	if (SelectedOptionNode) SelectedOptionNode->SetSelected();
}

void UInteractionManager::EndInteraction()
{
	if (SelectedOptionNode)
	{
		SelectedOptionNode->SetUnselected();
		SelectedOptionNode = nullptr;
	}
	
	if (InteractionMenuActor)
	{
		InteractionMenuActor->SetFollowActor(nullptr);
		InteractionMenuActor->CloseInteractionDialog();
	}

	if (CharacterWeAreInteractingWith)
	{
		CharacterWeAreInteractingWith->ForceTask(EmptyTask);
		CharacterWeAreInteractingWith = nullptr;
	}
	else if (PlotWeAreInteractingWith)
	{
		PlotWeAreInteractingWith = nullptr;
	}

	
	bIsInteracting = false;
	/*
	 *	un-hide npc's nametag
	 *
	 *	if npc is still waiting
	 *		order npc to carry on (DA_Empty)
	*/
}

void UInteractionManager::CycleOptions(const int Direction)
{
	if (!SelectedOptionNode)
	{
		return;
	}

	SelectedOptionNode->SetUnselected();
	
	if (Direction > 0) SelectedOptionNode = SelectedOptionNode->GetUpNode();
	else if (Direction < 0) SelectedOptionNode = SelectedOptionNode->GetDownNode();
	
	if (SelectedOptionNode) SelectedOptionNode->SetSelected();
}

void UInteractionManager::ConfirmOption()
{
	// run task associated with option
	if (!SelectedOptionNode) return;
	
	if (CharacterWeAreInteractingWith && SelectedOptionNode->OrderTask.IsValid())
	{
		CharacterWeAreInteractingWith->ForceTask(SelectedOptionNode->OrderTask);
		CharacterWeAreInteractingWith = nullptr;
	}
	else if (PlotWeAreInteractingWith)
	{
		const FBuildingInfo* Info = static_cast<const FBuildingInfo*>(SelectedOptionNode->ObjectTypeInfo);
		check(Info);
		TSubclassOf<AActor> Actor = Info->BuildingMesh;
		PlotWeAreInteractingWith->SetBuilding(Actor, Info);
		AActor* TempActorRef = PlotWeAreInteractingWith;
		EndInteraction();
		StartInteraction(TempActorRef);
	}
}

TSoftObjectPtr<UTask> UInteractionManager::GetUpgradeTaskForTool(const FToolInfo* ToolInfo)
{
	if (ToolInfo->ToolTag.GetTagName() == "Tool.Melee")
	{
		return PromoteMeleeTask;
	}
	if (ToolInfo->ToolTag.GetTagName() == "Tool.Ranged")
	{
		return PromoteRangedTask;
	}
	return PromoteBuilderTask;
}

TSoftObjectPtr<UTask> UInteractionManager::GetPromotionTaskForClass(const FClassInfo* ClassInfo)
{
	const FToolInfo* BaseTool = ClassInfo->BaseTool.GetRow<FToolInfo>(TEXT("help ;|"));
	return GetUpgradeTaskForTool(BaseTool);
}
