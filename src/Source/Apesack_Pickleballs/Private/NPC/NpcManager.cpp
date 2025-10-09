#include "NPC/NpcManager.h"

#include "NPC/NpcCharacter.h"
#include "UI/GridNode.h"
#include "UI/InteractionMenuActor.h"

UNpcManager::UNpcManager()
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
}

UNpcManager* UNpcManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UNpcManager>();
		}
	}
	return nullptr;
}

void UNpcManager::SetNpcClass(ANpcCharacter* NpcActor, FClassInfo* ClassInfo)
{
	
}

void UNpcManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InteractionMenuActor = InWorld.SpawnActor<AInteractionMenuActor>(AInteractionMenuActor::StaticClass());
	if (!InteractionMenuActor) UE_LOG(LogTemp, Error, TEXT("UNpcManager::OnWorldBeginPlay -> Failed to spawn InteractionMenuActor!"));
}

void UNpcManager::StartNpcInteraction(ANpcCharacter* NpcActor)
{
	if (bIsInteractingWithNpc || !InteractionMenuActor || !NpcActor) return;
	bIsInteractingWithNpc = true;
	
	InteractionMenuActor->OpenInteractionDialog(NpcActor);
	InteractionMenuActor->SetFollowActor(NpcActor);
	SelectedOptionNode = InteractionMenuActor->GetMostRelevantNode();
	if (SelectedOptionNode) SelectedOptionNode->SetSelected();
	CharacterWeAreInteractingWith = NpcActor;
	// Hide npc's nametag

	// Order npc to wait (DA_Wait)
	if (WaitTask.IsValid()) NpcActor->ForceTask(WaitTask);

}

void UNpcManager::EndNpcInteraction()
{
	if (InteractionMenuActor)
	{
		InteractionMenuActor->SetFollowActor(nullptr);
		InteractionMenuActor->CloseInteractionDialog();
	}

	if (CharacterWeAreInteractingWith) CharacterWeAreInteractingWith->ForceTask(EmptyTask);
	
	bIsInteractingWithNpc = false;
	/*
	 *	un-hide npc's nametag
	 *
	 *	if npc is still waiting
	 *		order npc to carry on (DA_Empty)
	*/
}

void UNpcManager::CycleOptions(const int Direction)
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

void UNpcManager::ConfirmOption()
{
	// run task associated with option
	if (SelectedOptionNode && CharacterWeAreInteractingWith)
	{
		CharacterWeAreInteractingWith->ForceTask(SelectedOptionNode->OrderTask);
		CharacterWeAreInteractingWith = nullptr;
	}
}

TSoftObjectPtr<UTask> UNpcManager::GetUpgradeTaskForTool(const FToolInfo* ToolInfo)
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

TSoftObjectPtr<UTask> UNpcManager::GetPromotionTaskForClass(const FClassInfo* ClassInfo)
{
	const FToolInfo* BaseTool = ClassInfo->BaseTool.GetRow<FToolInfo>(TEXT("help ;|"));
	return GetUpgradeTaskForTool(BaseTool);
}
