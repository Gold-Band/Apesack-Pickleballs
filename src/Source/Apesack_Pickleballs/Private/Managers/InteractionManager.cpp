#include "Managers/InteractionManager.h"

#include "Buildings/Plot.h"
#include "NPC/NpcCharacter.h"
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
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Actions/DA_WaitTask.DA_WaitTask"));
		if (TaskFinder.Succeeded())
		{
			WaitTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Actions/DA_EmptyTask.DA_EmptyTask"));
		if (TaskFinder.Succeeded())
		{
			EmptyTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Actions/DA_PromoteBuilder.DA_PromoteBuilder"));
		if (TaskFinder.Succeeded())
		{
			PromoteBuilderTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Actions/DA_PromoteMelee.DA_PromoteMelee"));
		if (TaskFinder.Succeeded())
		{
			PromoteMeleeTask = TaskFinder.Object;
		}
	}
	{
		ConstructorHelpers::FObjectFinder<UTask> TaskFinder(TEXT("/Game/HTN/Actions/DA_PromoteRanged.DA_PromoteRanged"));
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

void UInteractionManager::SetNpcClass(ANpcCharacter* NpcActor, FClassInfo* ClassInfo)
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

	if (ANpcCharacter* NpcActor = Cast<ANpcCharacter>(Actor))
	{
		// Order npc to wait (DA_Wait)
		if (WaitTask.IsValid()) NpcActor->ForceTask(WaitTask);
		// Save ref
		CharacterWeAreInteractingWith = NpcActor;
		// Make specialized dialogue
		InteractionMenuActor->OpenInteractionDialog(NpcActor);
		// Hide npc's nametag..?
	}
	else if (APlot* PlotActor = Cast<APlot>(Actor))
	{
		InteractionMenuActor->OpenInteractionDialog(PlotActor);
	}

	// generic
	InteractionMenuActor->SetFollowActor(Actor);
	SelectedOptionNode = InteractionMenuActor->GetMostRelevantNode();
	if (SelectedOptionNode) SelectedOptionNode->SetSelected();
}

void UInteractionManager::EndInteraction()
{
	if (InteractionMenuActor)
	{
		InteractionMenuActor->SetFollowActor(nullptr);
		InteractionMenuActor->CloseInteractionDialog();
	}

	if (CharacterWeAreInteractingWith) CharacterWeAreInteractingWith->ForceTask(EmptyTask);
	
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
		UE_LOG(LogTemp, Error, TEXT("CycleOptions - No Selected Node!"))
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
	if (SelectedOptionNode && CharacterWeAreInteractingWith && SelectedOptionNode->OrderTask.IsValid())
	{
		CharacterWeAreInteractingWith->ForceTask(SelectedOptionNode->OrderTask);
		CharacterWeAreInteractingWith = nullptr;
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
