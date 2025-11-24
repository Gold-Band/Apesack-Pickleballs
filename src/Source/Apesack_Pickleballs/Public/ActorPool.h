// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolableActor.h"


/**
 * 
 */
template <typename T>
class TActorPool
{
	//static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "TActorPool requires an AActor-derived type.");

public:
	TActorPool() = default;
	~TActorPool()
	{
		// delete actors
		for (T* Actor : AllActors)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		AllActors.Empty();
		//AvailableActors.Empty();
		while (!AvailableActors.IsEmpty())
		{
			T* Dummy;
			AvailableActors.Dequeue(Dummy);
		}
	}

	void Initialize(UWorld* InWorld, TSubclassOf<T> InActorClass, int InitialSize, bool bAllowAutoExpand = false, int AutoExpandSize = 10, const FActorSpawnParameters& InSpawnParameters = FActorSpawnParameters())
	{
		if (!InWorld || !InActorClass)
		{
			UE_LOG(LogTemp, Error, TEXT("TActorPool failed to initialize - InWorld or InActorClass is null!"))
			bInitSuccess = false;
			return;
		}
		bInitSuccess = true;

		World = InWorld;
		ActorClass = InActorClass;
		bAutoExpand = bAllowAutoExpand;
		ExpandChunkSize = FMath::Max(AutoExpandSize, 1);
		SpawnParameters = InSpawnParameters;
		
		// spawn actors
		ExpandBy(InitialSize);		
	}
	
	void ExpandBy(int ChunkSize)
	{
		for (int i = 0; i < ChunkSize; ++i)
		{
			auto NewActor = World->SpawnActor<T>(ActorClass, OutOfSightLocation,FRotator::ZeroRotator, SpawnParameters);

			check(NewActor);
			
			// Immediately deactivate – the pool has the actor
			auto Interface = Cast<IPoolableActor>(NewActor);
			Interface->GetOnActorDisabled().AddRaw(this, &TActorPool::OnActorDisabled);
			Interface->Disable();

			AllActors.Add(NewActor);
		}
	}
	
	void SetAutoExpand(bool bAllowAutoExpand)
	{
		bAutoExpand = bAllowAutoExpand;
	}

	T* GetActor()
	{
		T* Result = nullptr;
		if (!AvailableActors.Dequeue(Result) && bAutoExpand)
		{
			ExpandBy(ExpandChunkSize);
			AvailableActors.Dequeue(Result);
		}
		return Result;
	}

private:
	
	void OnActorDisabled(AActor* InActor)
	{
		// add it back to availables
		AvailableActors.Enqueue(InActor);
	}

	
	FVector OutOfSightLocation = FVector(0,0,-1000000);
	TObjectPtr<UWorld> World;
	TSubclassOf<T> ActorClass;
	TArray<T*> AllActors; 
	TQueue<T*> AvailableActors;
	FActorSpawnParameters SpawnParameters;
	
	bool bAutoExpand = false;
	bool bInitSuccess = false;

	int ExpandChunkSize = 10;
};

