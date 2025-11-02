// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTN/Task.h"
#include "MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UMoveTo : public UPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback) override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere)
	float StopDist = 50;
	UPROPERTY(EditAnywhere)
	float GotoDirectDistance = 500;
	UPROPERTY(EditAnywhere)
	float OutOfSightDistance = 1000;
	
private:
	float StopDistSquared;
	float GotoDirectDistanceSquared;
	float OutOfSightDistanceSquared;

	bool bOffRadius = false;
	
	UPROPERTY()
	APawn* InstigatorAsPawn = nullptr;
	UPROPERTY()
	AActor* TargetActor = nullptr;
	FVector TargetLocation;
	FVector DirToTarget;
};
