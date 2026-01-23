#include "Managers/NpcManager.h"

void FActorTernaryTree::Add(AActor* Actor, ENpcTag Tag)
{
	TUniquePtr<FNode>* CheckNode = &Root;

	while (true)
	{
		if (!CheckNode->IsValid()) // set
		{
			*CheckNode = MakeUnique<FNode>(Actor, Tag);
			return;
		}
	
		if (Tag < CheckNode->Get()->Tag) // left
		{
			CheckNode = &CheckNode->Get()->LeftChild;
		}
		else if (Tag == CheckNode->Get()->Tag) // middle
		{
			CheckNode = &CheckNode->Get()->MiddleChild;
		}
		else // right
		{
			CheckNode = &CheckNode->Get()->RightChild;
		}
	}
}

void FActorTernaryTree::Remove(AActor* Actor, ENpcTag Tag)
{
	
}

UNpcManager::UNpcManager()
{
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

AActor* UNpcManager::FindNearestNpc(FVector FromLocation, ENpcSearchOption SearchFilter)
{
	TArray<AActor*>* SearchArray = nullptr;
	
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		SearchArray = &AllHostiles;
		break;
	case ENpcSearchOption::AnyFriendly:
		SearchArray = &AllFriendlies;
		break;
	default: 
		return nullptr;
	}
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// sort objects array 
	if (SearchArray->Num() > 1)
	{
		SearchArray->Sort([FromLocation](const AActor& A, const AActor& B)
		{
			const float FastDistA = FVector::DistSquared(FromLocation, A.GetActorLocation());
			const float FastDistB = FVector::DistSquared(FromLocation, B.GetActorLocation());
			return (FastDistA > FastDistB);
		});
	}
	
	return AllHostiles[0];
}

void UNpcManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UNpcManager::SetWorldOrigin(const FVector& NewWorldOrigin)
{
	WorldOrigin = NewWorldOrigin;
}

void UNpcManager::AddNpc(AActor* Npc, ENpcTag Tag)
{
	//AllNpcs.Add(Npc, Tag);
	if (Tag == ENpcTag::Friendly)
	{
		AllFriendlies.Add(Npc);
	}
	else
	{
		AllHostiles.Add(Npc);
	}
}

void UNpcManager::RemoveNpc(AActor* Npc, ENpcTag Tag)
{
	//AllNpcs.Remove(Npc, Tag);
	if (Tag == ENpcTag::Friendly)
	{
		AllFriendlies.Remove(Npc);
	}
	else
	{
		AllHostiles.Remove(Npc);
	}
}

bool UNpcManager::SenseNpc(const FVector& FromLocation, ENpcSearchOption SearchFilter, float SenseRadiusSquared)
{
	TArray<AActor*>* SearchArray = nullptr;
	
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		SearchArray = &AllHostiles;
		break;
	case ENpcSearchOption::AnyFriendly:
		SearchArray = &AllFriendlies;
		break;
	default: 
		return false;
	}
	
	if (SearchArray->IsEmpty()) return false;
	
	if (SenseRadiusSquared > 0.0f)
	{
		for (const AActor* Actor : *SearchArray)
		{
			const float DistSquared = FVector::DistSquaredXY(FromLocation, Actor->GetActorLocation());
			if (DistSquared <= SenseRadiusSquared) return true;
		}
		return false;
	}
	
	return true;
}
