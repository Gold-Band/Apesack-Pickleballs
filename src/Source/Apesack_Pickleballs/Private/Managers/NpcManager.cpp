#include "Managers/NpcManager.h"

#include "AI/NPC/Npc.h"
#include "Buildings/Building.h"
#include "GameModes/DefaultGameMode.h"

/*void FActorTernaryTree::Add(AActor* Actor, ENpcTag Tag)
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
	
}*/


FOnMostVulnerableAssetChangedSignature UNpcManager::OnMostVulnerableAssetChangedDelegate; 
AActor* UNpcManager::LeftMostVulnerableAsset = nullptr;
AActor* UNpcManager::RightMostVulnerableAsset = nullptr;

FOnRaidDetectedSignature UNpcManager::OnRaidDetectedDelegate;



UNpcManager::UNpcManager()
{
	
}

ETickableTickType UNpcManager::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

TStatId UNpcManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(MyTickableClass, STATGROUP_Tickables);
}

void UNpcManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//*
	//* Left Side : Most Vulnerable Asset
	PreviousLeftMostVulnerableAsset = LeftMostVulnerableAsset;
	LeftMostVulnerableAsset = GetMostVulnerableAsset(EOriginSide::Left);
	if (LeftMostVulnerableAsset != PreviousLeftMostVulnerableAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("On LEFT changed -> %s"), LeftMostVulnerableAsset? *LeftMostVulnerableAsset->GetActorNameOrLabel() : TEXT("Nothing"));
		if (OnMostVulnerableAssetChangedDelegate.IsBound()) OnMostVulnerableAssetChangedDelegate.Broadcast(LeftMostVulnerableAsset, EOriginSide::Left);
	}
	
	//*
	//* Right Side : Most Vulnerable Asset
	PreviousRightMostVulnerableAsset = RightMostVulnerableAsset;
	RightMostVulnerableAsset = GetMostVulnerableAsset(EOriginSide::Right);
	if (RightMostVulnerableAsset != PreviousRightMostVulnerableAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("On RIGHT changed -> %s"), RightMostVulnerableAsset? *RightMostVulnerableAsset->GetActorNameOrLabel() : TEXT("Nothing"));
		if (OnMostVulnerableAssetChangedDelegate.IsBound()) OnMostVulnerableAssetChangedDelegate.Broadcast(RightMostVulnerableAsset, EOriginSide::Right);
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

void UNpcManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	BuildingsManager = UBuildingsManager::Get(GetWorld());
}

void UNpcManager::SetWorldOrigin(const FVector& NewWorldOrigin)
{
	WorldOrigin = NewWorldOrigin;
}

void UNpcManager::AddNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side)
{
	if (Tag == ENpcTag::Friendly)
	{
		if (Side == EOriginSide::Right) RightFriendlies.Add(Npc);
		else LeftFriendlies.Add(Npc);
	}
	else if (Tag == ENpcTag::Hostile)
	{
		if (Side == EOriginSide::Right) RightHostiles.Add(Npc);
		else LeftHostiles.Add(Npc);
	}
}

void UNpcManager::RemoveNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side)
{
	if (Tag == ENpcTag::Friendly)
	{
		if (Side == EOriginSide::Right) RightFriendlies.Remove(Npc);
		else LeftFriendlies.Remove(Npc);
	}
	else if (Tag == ENpcTag::Hostile)
	{
		if (Side == EOriginSide::Right) RightHostiles.Remove(Npc);
		else LeftHostiles.Remove(Npc);
	}
}

AActor* UNpcManager::FindNearestNpc(const FVector& FromLocation, ENpcSearchOption SearchFilter, EOriginSide Side)
{
	TArray<AActor*>* SearchArray = nullptr;
	
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		if (Side == EOriginSide::Right)	SearchArray = &RightHostiles;
		else SearchArray = &LeftHostiles;
		break;
	case ENpcSearchOption::AnyFriendly:
		if (Side == EOriginSide::Right) SearchArray = &RightFriendlies;
		else SearchArray = &LeftFriendlies;
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
	
	return (*SearchArray)[0];
}

AActor* UNpcManager::GetFarthestNpc(ENpcSearchOption SearchFilter, EOriginSide Side)
{
	TArray<AActor*>* SearchArray = nullptr;
	
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		if (Side == EOriginSide::Right)	SearchArray = &RightHostiles;
		else SearchArray = &LeftHostiles;
		break;
	case ENpcSearchOption::AnyFriendly:
		if (Side == EOriginSide::Right) SearchArray = &RightFriendlies;
		else SearchArray = &LeftFriendlies;
		break;
	default: 
		return nullptr;
	}
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// sort objects array 
	if (SearchArray->Num() > 1)
	{
		const FVector FromLocation = WorldOrigin * 10000; 
		SearchArray->Sort([FromLocation](const AActor& A, const AActor& B)
		{
			const float FastDistA = FVector::DistSquared(FromLocation, A.GetActorLocation());
			const float FastDistB = FVector::DistSquared(FromLocation, B.GetActorLocation());
			return (FastDistA < FastDistB);
		});
	}
	
	return (*SearchArray)[0];
}

AActor* UNpcManager::GetMostVulnerableAsset(const EOriginSide Side)
{
	// Get farthest Npc
	AActor* Npc = GetFarthestNpc(ENpcSearchOption::AnyFriendly, Side);

	// Get farthest wall
	ABuilding* Building = Cast<ABuilding>(BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, Side)); 
		
	if (Npc == nullptr) return Building;
	if (Building == nullptr) return Npc;
	
	// Compare
	const float NpcDistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(Npc->GetActorLocation());
	if (Side == EOriginSide::Left) return Building->DistanceFromOrigin < NpcDistanceFromOrigin ? Building : Npc;
	return Building->DistanceFromOrigin > NpcDistanceFromOrigin ? Building : Npc;
}
