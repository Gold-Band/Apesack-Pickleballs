// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TaskHelpers.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UTaskHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:
	
	UFUNCTION(BlueprintPure, Category = TaskHelpers)
	static float GetProjectileLaunchArcRatio(const UObject* WorldContextObject, const FVector& Start, const FVector& End, const TSubclassOf<AActor> FilterClass); 
	
};
