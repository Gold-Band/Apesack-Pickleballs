
#pragma once

#include "CoreMinimal.h"
#include "NpcManager.generated.h"


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
};

//*
//* Binary tree test
//*
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
};

//*
//*
//*


UCLASS()
class UNpcManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UNpcManager();
	
	static UNpcManager* Get(const UObject* WorldContextObject);

	AActor* FindNearestNpc(FVector FromLocation, ENpcSearchOption SearchFilter);
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void SetWorldOrigin(const FVector& NewWorldOrigin);	
	
	void AddNpc(AActor* Npc, ENpcTag Tag);
	void RemoveNpc(AActor* Npc, ENpcTag Tag);
	bool SenseNpc(const FVector& FromLocation, ENpcSearchOption SearchFilter, float SenseRadiusSquared);
	
private:
	FVector WorldOrigin;
	
	//FActorTernaryTree AllNpcs = FActorTernaryTree{}; // might not use this
	
	UPROPERTY()
	TArray<AActor*> AllHostiles;
	UPROPERTY()
	TArray<AActor*> AllFriendlies;
	
};
