// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/Actions/TaskHelpers.h"

float UTaskHelpers::GetProjectileLaunchArcRatio(const UObject* WorldContextObject, const FVector& Start, const FVector& End, const TSubclassOf<AActor> FilterClass)
{
	const UWorld* World = WorldContextObject->GetWorld();
	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionObjectParams(FCollisionObjectQueryParams::AllStaticObjects);
	if (World->LineTraceSingleByObjectType(HitResult, Start, End, CollisionObjectParams))
	{
		if (HitResult.GetActor()->IsA(FilterClass))
		{
			return 0.2f;
		}
	}
	
	return 0.75f;
}
