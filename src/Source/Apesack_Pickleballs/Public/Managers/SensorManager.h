// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SensorManager.generated.h"

UCLASS(Blueprintable)
class APESACK_PICKLEBALLS_API USensorManager: public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static USensorManager* Get(const UObject* WorldContextObject);

	AActor* FindNearestWithTag(FGameplayTag Tag, FVector FromLocation);
	
	void AddObject(AActor* Object);
	void RemoveObject(AActor* Object);
	
private:
	UPROPERTY()
	TArray<AActor*> Objects;
};