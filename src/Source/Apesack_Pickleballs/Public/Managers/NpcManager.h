
#pragma once

#include "CoreMinimal.h"
#include "BuildingsManager.h"
#include "NpcManager.generated.h"


class APlayerCharacter;
class ANpc;
class ANpcFriendly;


UENUM(BlueprintType)
enum class ENpcSearchOption : uint8
{
	Any,
	AnyHostile,
	AnyFriendly
};

UENUM(BlueprintType)
enum class ENpcTag : uint8
{
	None = 0,
	Hostile = 1,
	Friendly = 2,
	Party = 3
};

/*
class FNode
{
public:
	explicit FNode(AActor* ActorPtr = nullptr, ENpcTag NpcTag = ENpcTag::None): Actor(ActorPtr), Tag(NpcTag), 
	LeftChild(nullptr), MiddleChild(nullptr), RightChild(nullptr) {}
	AActor* Actor;
	ENpcTag Tag;
	
	TUniquePtr<FNode> LeftChild;
	TUniquePtr<FNode> MiddleChild;
	TUniquePtr<FNode> RightChild;
};

class FActorTernaryTree
{
public:
	TUniquePtr<FNode> Root;
	
	void Add(AActor* Actor, ENpcTag Tag);
	void Remove(AActor* Actor, ENpcTag Tag);
	//void Sort();	
	//void Get();	
};*/



// delegate for when an npc or building is the newly most exposed to the enemy
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMostVulnerableAssetChangedSignature, AActor*, EOriginSide);

// delegate for when enemies are spotted near a wall
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRaidDetectedSignature, EOriginSide);



UCLASS()
class UNpcManager: public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UNpcManager();
	
	
	//* 
	//* Npc Delegates
	//*
	static FOnMostVulnerableAssetChangedSignature OnMostVulnerableAssetChangedDelegate;
	static AActor* LeftMostVulnerableAsset;
	static AActor* RightMostVulnerableAsset;
	
	static FOnRaidDetectedSignature OnRaidDetectedDelegate;

	virtual ETickableTickType GetTickableTickType() const override;
	
	virtual TStatId GetStatId() const override;
	
	virtual void Tick(float DeltaTime) override;
	
	static UNpcManager* Get(const UObject* WorldContextObject);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void SetWorldOrigin(const FVector& NewWorldOrigin);	
	
	void AddNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side);
	void RemoveNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side);
	AActor* FindNearestNpc(const FVector& FromLocation, const ENpcSearchOption SearchFilter, const EOriginSide Side = EOriginSide::Any, const float CheckRadiusSquared = UE_MAX_FLT);
	AActor* FindNearestNpcOrPlayer(const FVector& FromLocation, const ENpcSearchOption SearchFilter, const EOriginSide Side = EOriginSide::Any, const float CheckRadiusSquared = UE_MAX_FLT);
	
	AActor* GetFarthestFriendlyNpc(EOriginSide Side);
	
	void SortByOriginAngle(TArray<AActor*>* SortArray);
	
	TArray<AActor*> GetNpcs(ENpcSearchOption SearchFilter, EOriginSide Side) const;
	
	EOriginSide SuggestOccupySide() const;
	
	float RaidDetectionDistance = 10000;
	
	APlayerCharacter* GetPlayer() const;
	
private:
	// Returns CheckActor if it passes the inspection. Else, it returns null.
	bool IsActorValidNearest(const AActor* CheckActor, const EOriginSide CheckSide, const float CheckDist, const float CheckRadiusSquared) const;
	bool IsCorrectSide(const EOriginSide Side, const FVector& WorldLocation) const;
	
	TArray<AActor*>* GetArray(ENpcSearchOption SearchFilter);
	
	AActor* GetMostVulnerableAsset(const EOriginSide Side);
	
	UPROPERTY()
	AActor* PreviousLeftMostVulnerableAsset;
	
	UPROPERTY()
	AActor* PreviousRightMostVulnerableAsset; 
	
	UPROPERTY()
	UBuildingsManager* BuildingsManager;
	
	FVector WorldOrigin;
	
	UPROPERTY()
	TArray<AActor*> AllHostiles;
	
	UPROPERTY()
	TArray<AActor*> AllFriendlies;
	
	//UPROPERTY()
	//TArray<AActor*> PartyMembers;
	
	
	// tick interval
	float TickInterval = 0.2f;
	float Timer;
	
	APlayerCharacter* PlayerRef = nullptr;
	
};
