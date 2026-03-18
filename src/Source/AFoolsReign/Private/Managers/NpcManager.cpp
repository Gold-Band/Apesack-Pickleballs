#include "Managers/NpcManager.h"

#include "AI/NPC/NpcCultist.h"
#include "AFoolsReign/PlayerCharacter.h"
#include "Buildings/Building.h"
#include "Buildings/Wall.h"
#include "GameModes/DefaultGameMode.h"

FOnMostVulnerableAssetChangedSignature UNpcManager::OnMostVulnerableAssetChangedDelegate; 
AActor* UNpcManager::LeftMostVulnerableAsset = nullptr;
AActor* UNpcManager::RightMostVulnerableAsset = nullptr;

FOnRaidDetectedSignature UNpcManager::OnRaidDetectedDelegate;



UNpcManager::UNpcManager()
{
	// get enemy classes
	
	static ConstructorHelpers::FClassFinder<ANpcCultist> CultistClassFinder{TEXT("/Game/Blueprints/Characters/BP_Cultist")};
	if (CultistClassFinder.Succeeded())
	{
		CultistClass = CultistClassFinder.Class;
	}
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
	
	const float PlayerDist = PlayerRef? ADefaultGameMode::GetAngleToOrigin(PlayerRef->GetActorLocation()) : 0;
	
	//*
	//* Left Side : Most Vulnerable Asset
	PreviousLeftMostVulnerableAsset = LeftMostVulnerableAsset;
	float Dist = GetMostVulnerableAssetAndDistance(EOriginSide::Left, LeftMostVulnerableAsset);
	RefreshNearbyVulnerables(EOriginSide::Left);
	if (PlayerDist < Dist) LeftMostVulnerableAsset = PlayerRef;
	if (LeftMostVulnerableAsset != PreviousLeftMostVulnerableAsset)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("On LEFT changed -> %s"), LeftMostVulnerableAsset? *LeftMostVulnerableAsset->GetActorNameOrLabel() : TEXT("Nothing"));
#endif
		if (OnMostVulnerableAssetChangedDelegate.IsBound()) OnMostVulnerableAssetChangedDelegate.Broadcast(LeftMostVulnerableAsset, EOriginSide::Left);
	}
	
	//if (LeftMostVulnerableAsset) DrawDebugLine(GetWorld(), FVector{0,19000.0,0}, LeftMostVulnerableAsset->GetActorLocation(), FColor::Green, false, TickInterval);
	
	//*
	//* Right Side : Most Vulnerable Asset
	PreviousRightMostVulnerableAsset = RightMostVulnerableAsset;
	Dist = GetMostVulnerableAssetAndDistance(EOriginSide::Right, RightMostVulnerableAsset);
	RefreshNearbyVulnerables(EOriginSide::Right);
	if (PlayerDist > Dist) RightMostVulnerableAsset = PlayerRef;
	if (RightMostVulnerableAsset != PreviousRightMostVulnerableAsset)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("On RIGHT changed -> %s"), RightMostVulnerableAsset? *RightMostVulnerableAsset->GetActorNameOrLabel() : TEXT("Nothing"));
#endif
		if (OnMostVulnerableAssetChangedDelegate.IsBound()) OnMostVulnerableAssetChangedDelegate.Broadcast(RightMostVulnerableAsset, EOriginSide::Right);
	}
	
	//if (RightMostVulnerableAsset) DrawDebugLine(GetWorld(), FVector{0,19000.0,0}, RightMostVulnerableAsset->GetActorLocation(), FColor::Green, false, TickInterval);
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
	PlayerRef = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	BuildingsManager->OnNewWallBuiltDelegate.AddLambda([&](const AWall* NewWall, const EOriginSide Side){ RecalculateSafeZone(Side, NewWall->DistanceFromOrigin); });
	BuildingsManager->OnWallDestroyedDelegate.AddLambda([&](const AWall* NewWall, const EOriginSide Side){ RecalculateSafeZone(Side, NewWall->DistanceFromOrigin); });
	
	
	//ReDrawSafeZoneBounds();
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
	const float CheckRadiusSquared)
{
	TArray<AActor*>* SearchArray = GetArray(SearchFilter);
	
	if (SearchArray->IsEmpty()) return nullptr;
	
	// assume the array is sorted from left npcs -> right npcs
	if (SearchArray->Num() > 1)
	{
		int ContinueSearchFromIndex = -1;
		
		for (int a = FMath::DivideAndRoundDown(SearchArray->Num(), 2), Closest = -1; SearchArray->IsValidIndex(a) ; )
		{
			const int b = a-1 >= 0? a-1 : a+1 < SearchArray->Num()? a+1 : -1;
			if (b == -1) break;
		
			// compare left and right
			const float FastDistA = FVector::DistSquared(FromLocation, (*SearchArray)[a]->GetActorLocation());
			const float FastDistB = FVector::DistSquared(FromLocation, (*SearchArray)[b]->GetActorLocation());
			
			if (FastDistA > FastDistB)
			{
				if (Closest == b) // found the closest
				{
					// check b - the closest
					AActor* ClosestActor = (*SearchArray)[b];
					if (IsActorValidNearest(ClosestActor, Side, FastDistB, CheckRadiusSquared)) return ClosestActor;
					
					// start moving left or right until we find an actor that passes the inspection
					ContinueSearchFromIndex = Closest;
					break;
				}
				Closest = b;
				a--;
			}
			else
			{
				if (Closest == a) // found the closest
				{
					// check a - the closest
					AActor* ClosestActor = (*SearchArray)[a];
					if (IsActorValidNearest(ClosestActor, Side, FastDistA, CheckRadiusSquared)) return ClosestActor;
					
					// start moving left or right until we find an actor that passes the inspection
					ContinueSearchFromIndex = Closest;
					break;
				}
				Closest = a;
				a++;
			}
			
		}
		
		// if we found the closest but it's not on the correct side
		if (ContinueSearchFromIndex != -1)
		{
			for (int i = Side == EOriginSide::Right? ContinueSearchFromIndex+1 : ContinueSearchFromIndex-1; 
				SearchArray->IsValidIndex(i); i = Side == EOriginSide::Right? i+1 : i-1)
			{
				AActor* CheckActor = (*SearchArray)[i];
				const float FastDist = FVector::DistSquared(FromLocation, CheckActor->GetActorLocation());
				
				// is Actor out of range?
				if (FastDist > CheckRadiusSquared) break;
				
				// skip if incorrect side
				if (!IsCorrectSide(Side, CheckActor->GetActorLocation())) continue;
				
				return CheckActor;
			}
			
			return nullptr;	
		}
		
		// the last index is the closest but needs a final inspection 
		AActor* CheckActor = SearchArray->Last();
		const float FastDist = FVector::DistSquared(FromLocation, CheckActor->GetActorLocation());
		if (IsActorValidNearest(CheckActor, Side, FastDist, CheckRadiusSquared)) return CheckActor;
		return nullptr;
	}
	
	// inspection for the one item in the array
	AActor* CheckActor = (*SearchArray)[0];
	const float FastDist = FVector::DistSquared(FromLocation, CheckActor->GetActorLocation());
	if (IsActorValidNearest(CheckActor, Side, FastDist, CheckRadiusSquared)) return CheckActor;
	return nullptr;
}

AActor* UNpcManager::FindNearestNpcOrPlayer(const FVector& FromLocation, const ENpcSearchOption SearchFilter,
	const EOriginSide Side, const float CheckRadiusSquared)
{
	AActor* FoundNpc = FindNearestNpc(FromLocation, SearchFilter, Side, CheckRadiusSquared);
	
	AActor* Player = GetWorld()->GetGameInstance()->GetFirstLocalPlayerController()->GetPawn();
	
	const float PlayerDist = Player? FVector::DistSquaredXY(FromLocation, Player->GetActorLocation()) : UE_MAX_FLT;
	const float NpcDist = FoundNpc? FVector::DistSquaredXY(FromLocation, FoundNpc->GetActorLocation()) : UE_MAX_FLT;
	
	//if (!FoundNpc) return Player;
	//if (!Player) return FoundNpc;
	
	if (PlayerDist < CheckRadiusSquared && PlayerDist < NpcDist) return Player;
	if (NpcDist < CheckRadiusSquared && NpcDist < PlayerDist) return FoundNpc;
	
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
			//DrawDebugLine(GetWorld(), FromLocation, AllFriendlies.Last()->GetActorLocation(), FColor::Green, false, TickInterval);
			return AllFriendlies.Last();
		}
	}
	
	if (Side == EOriginSide::Left)
	{
		const float ActualAngle = ADefaultGameMode::GetAngleBetweenVectors(FromLocation, AllFriendlies[0]->GetActorLocation());
		if (ActualAngle > 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Left - %f"), ActualAngle);
			//DrawDebugLine(GetWorld(), FromLocation, AllFriendlies[0]->GetActorLocation(), FColor::Green, false, TickInterval);
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

APlayerCharacter* UNpcManager::GetPlayer() const
{
	return PlayerRef;
}

float UNpcManager::GetMaxSafeAngle(const EOriginSide Side) const
{
	return Side == EOriginSide::Left? MaxSafeAngles.X : MaxSafeAngles.Y;
}

void UNpcManager::OnCultistDied(const EOriginSide Side)
{
	if (Side == EOriginSide::Any) return;
	
	// spawn a new cultist
	const FVector Axis = Side == EOriginSide::Left ? FVector::UpVector : FVector::DownVector;
	const float Radius = ADefaultGameMode::GameplayRadius + FMath::RandRange(-80,80);
	const FVector SpawnLocation = ADefaultGameMode::WorldOriginNormal.RotateAngleAxis(179, Axis).GetClampedToSize2D(Radius, Radius) + FVector::UpVector * 60.0f;

	GetWorld()->SpawnActor(CultistClass.Get(), &SpawnLocation);
}

AActor* UNpcManager::GetAttackable(const EOriginSide Side, const ENpcSearchOption Filter)
{
	switch (Filter)
	{
	case ENpcSearchOption::Any:
		break;
	case ENpcSearchOption::AnyHostile:
		
		break;
	case ENpcSearchOption::AnyFriendly:
		if (Side == EOriginSide::Left) return LeftVulnerables.IsEmpty()? LeftMostVulnerableAsset: LeftVulnerables[FMath::RandRange(0, LeftVulnerables.Num()-1)];
		return RightVulnerables.IsEmpty()? RightMostVulnerableAsset: RightVulnerables[FMath::RandRange(0, RightVulnerables.Num()-1)];
	default: ;
	}
	
	return nullptr;
}

bool UNpcManager::IsActorValidNearest(const AActor* CheckActor, const EOriginSide CheckSide, const float CheckDist,
                                      const float CheckRadius) const
{
	if (CheckDist <= CheckRadius && 
				IsCorrectSide(CheckSide, CheckActor->GetActorLocation()))
	{
		return true;
	}
	return false;	
}


bool UNpcManager::IsCorrectSide(const EOriginSide Side, const FVector& WorldLocation) const
{
	if (Side == EOriginSide::Any) return true;
	
	const float Angle = ADefaultGameMode::GetAngleToOrigin(WorldLocation);
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

float UNpcManager::GetMostVulnerableAssetAndDistance(const EOriginSide Side, AActor*& OutActor)
{
	// Get farthest Npc	
	AActor* Npc = GetFarthestFriendlyNpc(Side);

	// Get farthest wall
	ABuilding* Building = Cast<ABuilding>(BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, Side)); 
	
	// if one of them is null
	if (!Npc || !Building)
	{
		OutActor = nullptr;
		
		if (Npc == nullptr) OutActor = Building;
		else bMostVulnerableIsAWall = false;
		
		if (Building == nullptr) OutActor = Npc;
		else bMostVulnerableIsAWall = true;
		
		return OutActor? ADefaultGameMode::GetAngleToOrigin(OutActor->GetActorLocation()) : 0;
	}
	
	// Compare
	const float NpcDistanceFromOrigin = ADefaultGameMode::GetAngleToOrigin(Npc->GetActorLocation());
	
	if (Side == EOriginSide::Left)
	{
		if (Building->DistanceFromOrigin < NpcDistanceFromOrigin)
		{
			OutActor = Building;
			bMostVulnerableIsAWall = true;
			return Building->DistanceFromOrigin;
		}
		OutActor = Npc;
		bMostVulnerableIsAWall = false;
		return ADefaultGameMode::GetAngleToOrigin(Npc->GetActorLocation());
	}
	
	if (Building->DistanceFromOrigin > NpcDistanceFromOrigin)
	{
		OutActor = Building;
		bMostVulnerableIsAWall = true;
		return Building->DistanceFromOrigin;
	}
	OutActor = Npc;
	bMostVulnerableIsAWall = false;
	return ADefaultGameMode::GetAngleToOrigin(Npc->GetActorLocation());
}

void UNpcManager::RefreshNearbyVulnerables(const EOriginSide Side)
{
	TArray<AActor*>* ModifyArray;
	
	if (Side == EOriginSide::Left) ModifyArray = &LeftVulnerables;
	else ModifyArray = &RightVulnerables;
	
	if (bMostVulnerableIsAWall)
	{
		ModifyArray->Empty();
		return;
	}
	
	const AActor* Npc = Side == EOriginSide::Left ? LeftMostVulnerableAsset : RightMostVulnerableAsset;
	if (!Npc) return;
	constexpr float Range = 1.f;
	
	// Debug draw the range
/*#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Npc->GetActorLocation(), Npc->GetActorLocation() + FVector::UpVector * 150.f, FColor::Yellow, false, 0.2);
	const FVector RangeEnd = Npc->GetActorLocation().RotateAngleAxis(Range, Side == EOriginSide::Right ? FVector::UpVector : FVector::DownVector);
	DrawDebugLine(GetWorld(), RangeEnd, RangeEnd + FVector::UpVector * 150.f, FColor::Yellow, false, 0.2);
#endif
	//*/
	
	
	// to make the for loop look nice :)
	const auto Condition = [Range, Npc] (const AActor* Other) { return FMath::Abs(ADefaultGameMode::GetAngleBetweenVectors(Npc->GetActorLocation(),Other->GetActorLocation())) <= Range;};
	const auto Start = [Side] (const TArray<AActor*>& Arr){ return Side==EOriginSide::Left?0:Arr.Num()-1;};
	const auto Next = [Side](int& Out){return Side==EOriginSide::Left?Out++:Out--;};
	
	// this is npc array. look back
	for (int i = Start(AllFriendlies); AllFriendlies.IsValidIndex(i) && Condition(AllFriendlies[i]); Next(i))
	{
		//if (AllFriendlies[i] == Npc) continue;
		ModifyArray->Add(AllFriendlies[i]);
		
		// make it clear what was added
/*#if WITH_EDITOR
		const AActor* Added = AllFriendlies[i];
		//UE_LOG(LogTemp, Warning, TEXT("Adding %s to most vulnerables"), *Added->GetActorLabel())
		DrawDebugLine(GetWorld(), Added->GetActorLocation(), Added->GetActorLocation() + FVector::UpVector * 100.f, FColor::Green, false, 0.2f);
#endif*/
	}
}

void UNpcManager::RecalculateSafeZone(const EOriginSide Side, const float FarthestWallAngle)
{
	const float Angle = FMath::Abs(FarthestWallAngle) - SafeZoneWallPaddingAngle;
	if (Side == EOriginSide::Left) MaxSafeAngles.X = Angle;
	else MaxSafeAngles.Y = Angle;
	
	//ReDrawSafeZoneBounds();	
}

void UNpcManager::ReDrawSafeZoneBounds() const
{
#if WITH_EDITOR
	FlushPersistentDebugLines(GetWorld());
	const FVector Origin = ADefaultGameMode::WorldOriginNormal * ADefaultGameMode::GameplayRadius;
	const FVector Max1 = Origin.RotateAngleAxis(MaxSafeAngles.X, FVector::UpVector);
	const FVector Max2 = Origin.RotateAngleAxis(-MaxSafeAngles.Y, FVector::UpVector);
	const FVector Min = Origin;
	DrawDebugLine(GetWorld(), Max1, Max1 + FVector::UpVector * 100.0f, FColor::Red, true);
	DrawDebugLine(GetWorld(), Min, Min + FVector::UpVector * 100.0f, FColor::Yellow, true);
	DrawDebugLine(GetWorld(), Max2, Max2 + FVector::UpVector * 100.0f, FColor::Red, true);
#endif
}
