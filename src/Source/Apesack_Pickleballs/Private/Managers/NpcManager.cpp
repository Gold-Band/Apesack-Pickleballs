#include "Managers/NpcManager.h"

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
	
	Timer+=DeltaTime;
	
	if (Timer < TickInterval) return;
	Timer = 0;
	
	//* Sort
	SortByOriginAngle(&AllFriendlies);
	SortByOriginAngle(&AllHostiles);
	
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
		AllFriendlies.Add(Npc);
	}
	else if (Tag == ENpcTag::Hostile)
	{
		AllHostiles.Add(Npc);
	}
}

void UNpcManager::RemoveNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side)
{
	if (Tag == ENpcTag::Friendly)
	{
		AllFriendlies.Remove(Npc);
	}
	else if (Tag == ENpcTag::Hostile)
	{
		AllHostiles.Remove(Npc);
	}
}

AActor* UNpcManager::FindNearestNpc(const FVector& FromLocation, const ENpcSearchOption SearchFilter, const EOriginSide Side,
	const float SearchRadius)
{
	TArray<AActor*>* SearchArray = GetArray(SearchFilter);
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// assume the array is sorted from left npcs -> right npcs
	if (SearchArray->Num() > 1)
	{
		for (int a = SearchArray->Num()/2, Closest = -1; SearchArray->IsValidIndex(a) ; )
		{
			const int b = a-1 >= 0? a-1 : a+1 < SearchArray->Num()? a+1 : -1;
			if (b == -1) break;
		
			// compare left and right
			const float FastDistA = FVector::DistSquared2D(FromLocation, (*SearchArray)[a]->GetActorLocation());
			const float FastDistB = FVector::DistSquared2D(FromLocation, (*SearchArray)[b]->GetActorLocation());
			
			// check closest
			if (FastDistA > FastDistB)
			{
				if (Closest == b && FastDistB <= FMath::Square(SearchRadius))
				{
					if (IsCorrectSide(Side, (*SearchArray)[b]->GetActorLocation()))
					{
						return (*SearchArray)[b];
					}
					// else check a
					if (IsCorrectSide(Side, (*SearchArray)[a]->GetActorLocation()))
					{
						return (*SearchArray)[a];
					}
					// if not a then no solution
					return nullptr;
				}
				Closest = b;
				a--;
			}
			else
			{
				if (Closest == a && FastDistA <= FMath::Square(SearchRadius))
				{
					if (IsCorrectSide(Side, (*SearchArray)[a]->GetActorLocation()))
					{
						return (*SearchArray)[a];
					}
					// else check b
					if (IsCorrectSide(Side, (*SearchArray)[b]->GetActorLocation()))
					{
						return (*SearchArray)[b];
					}
					// if not b then no solution
					return nullptr;
				}
				Closest = a;
				a++;
			}
		}
		if (IsCorrectSide(Side, SearchArray->Last()->GetActorLocation())) return SearchArray->Last();
		return nullptr;
	}
	if (IsCorrectSide(Side, (*SearchArray)[0]->GetActorLocation())) return (*SearchArray)[0];
	return nullptr;
}

AActor* UNpcManager::GetFarthestFriendlyNpc(EOriginSide Side)
{
	if (AllFriendlies.IsEmpty()) return nullptr;
	
	// assumes AllFriendlies is sorted
	
	const FVector FromLocation = FVector{0,19000, 0};
	
	if (Side == EOriginSide::Right)
	{
		const float ActualAngle = ADefaultGameMode::GetAngleBetweenVectors(FromLocation, AllFriendlies.Last()->GetActorLocation());
		if (ActualAngle < 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Right - %f"), ActualAngle);
			DrawDebugLine(GetWorld(), FromLocation, AllFriendlies.Last()->GetActorLocation(), FColor::Green, false, TickInterval);
			return AllFriendlies.Last();
		}
	}
	
	if (Side == EOriginSide::Left)
	{
		const float ActualAngle = ADefaultGameMode::GetAngleBetweenVectors(FromLocation, AllFriendlies[0]->GetActorLocation());
		if (ActualAngle > 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Left - %f"), ActualAngle);
			DrawDebugLine(GetWorld(), FromLocation, AllFriendlies[0]->GetActorLocation(), FColor::Green, false, TickInterval);
			return AllFriendlies[0];
		}
	}

	return nullptr;
}

void UNpcManager::SortByOriginAngle(TArray<AActor*>* SortArray)
{
	const FVector FromLocation = FVector{0,19000.0f, 0}; 
	SortArray->Sort([FromLocation](const AActor& A, const AActor& B)
	{
		const float AngleA = ADefaultGameMode::GetAngleBetweenVectors(FromLocation, A.GetActorLocation());
		const float AngleB = ADefaultGameMode::GetAngleBetweenVectors(FromLocation, B.GetActorLocation());
		return AngleA > AngleB;
	});
}

TArray<AActor*> UNpcManager::GetNpcs(ENpcSearchOption SearchFilter, EOriginSide Side) const
{
	const TArray<AActor*>* SearchArray = nullptr;
	
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		SearchArray = &AllHostiles;
		break;
	case ENpcSearchOption::AnyFriendly:
		SearchArray = &AllFriendlies;
		break;
	default:; 
	}
	
	return *SearchArray;
}

EOriginSide UNpcManager::SuggestOccupySide() const
{
	if (AllFriendlies.Num() % 2 == 0) return EOriginSide::Left;
	return EOriginSide::Right;
}

bool UNpcManager::IsCorrectSide(const EOriginSide Side, const FVector& WorldLocation) const
{
	if (Side == EOriginSide::Any) return true;
	
	const float Angle = ADefaultGameMode::GetDistanceToOrigin(WorldLocation);
	if ((Angle < 0 && Side == EOriginSide::Left) || 
		(Angle >= 0 && Side == EOriginSide::Right))
	{
		return true;
	}
	return false;
}

TArray<AActor*>* UNpcManager::GetArray(ENpcSearchOption SearchFilter)
{
	switch (SearchFilter)
	{
	case ENpcSearchOption::AnyHostile:
		return &AllHostiles;
	case ENpcSearchOption::AnyFriendly:
		return &AllFriendlies;
	default: 
		return nullptr;
	}
}

AActor* UNpcManager::GetMostVulnerableAsset(const EOriginSide Side)
{
	// Get farthest Npc
	AActor* Npc = GetFarthestFriendlyNpc(Side);

	// Get farthest wall
	ABuilding* Building = Cast<ABuilding>(BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, Side)); 
		
	if (Npc == nullptr) return Building;
	if (Building == nullptr) return Npc;
	
	// Compare
	const float NpcDistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(Npc->GetActorLocation());
	if (Side == EOriginSide::Left) return Building->DistanceFromOrigin < NpcDistanceFromOrigin ? Building : Npc;
	return Building->DistanceFromOrigin > NpcDistanceFromOrigin ? Building : Npc;
}
