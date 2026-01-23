#include "Managers/InteractionManager.h"

#include "Buildings/Plot.h"
#include "GameFramework/GameModeBase.h"
#include "GameModes/DefaultGameMode.h"
#include "AI/NPC/NpcFriendly.h"
#include "UI/GridNode.h"
//#include "AI/HTN/Task.h"
#include "UI/InteractionMenuActor.h"

UInteractionManager::UInteractionManager()
{
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

	GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("UInteractionManager::OnWorldBeginPlay - Failed to get game mode!"))		
		return;
	}
	
	AllClasses = GameMode->GetAllClasses();
	AllBuildings = GameMode->GetAllBuildings();
}

void UInteractionManager::StartInteraction(AActor* Actor)
{

	if (bIsInteracting || !InteractionMenuActor || !Actor) return;
	bIsInteracting = true;

	if (ANpcFriendly* NpcActor = Cast<ANpcFriendly>(Actor))
	{
		
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
	
	//if (CharacterWeAreInteractingWith && SelectedOptionNode->OrderTask.IsValid())
	//{
	//	CharacterWeAreInteractingWith = nullptr;
	//}
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