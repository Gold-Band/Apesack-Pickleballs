// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTN/Task.h"
#include "FindNearestActorWithTag.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UFindNearestActorWithTag : public UPrimitiveTask
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;
	UPROPERTY(EditAnywhere)
	FName SearchTag = "";
};
