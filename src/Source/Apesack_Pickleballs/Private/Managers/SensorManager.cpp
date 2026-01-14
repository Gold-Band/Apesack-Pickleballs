// Fill out your copyright notice in the Description page of Project Settings.
#include "Managers/SensorManager.h"
#include "GameplayTagAssetInterface.h"

USensorManager* USensorManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			const UGameInstance* GameInstance = World->GetGameInstance();
			if (GameInstance) return GameInstance->GetSubsystem<USensorManager>();
		}
	}
	return nullptr;
}

AActor* USensorManager::FindNearestWithTag(FGameplayTag Tag, FVector FromLocation)
{
	if (Objects.IsEmpty()) return nullptr;
	
	// sort objects array 
	if (Objects.Num() > 1)
	{
		Objects.Sort([FromLocation](const AActor& A, const AActor& B)
		{
			const float FastDistA = FVector::DistSquared(FromLocation, A.GetActorLocation());
			const float FastDistB = FVector::DistSquared(FromLocation, B.GetActorLocation());
			return (FastDistA > FastDistB);
		});
	}
	
	for (AActor* Object : Objects)
	{
		const auto Interface = Cast<IGameplayTagAssetInterface>(Object);
		if (Interface->HasMatchingGameplayTag(Tag))
		{
			return Object;
		}
	}

	return nullptr;
}

void USensorManager::AddObject(AActor* Object)
{
	Objects.Add(Object);
}

void USensorManager::RemoveObject(AActor* Object)
{
	Objects.Remove(Object);
}
