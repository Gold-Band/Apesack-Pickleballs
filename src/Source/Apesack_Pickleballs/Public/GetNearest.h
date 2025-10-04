// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTN/Task.h"
#include "GetNearest.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGetNearest : public UPrimitiveTask
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ObjectTag;
};
