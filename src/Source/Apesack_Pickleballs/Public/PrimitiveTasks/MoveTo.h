// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTN/Task.h"
#include "MoveTo.generated.h"

class ANpcBase;
class UHTNComponent;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UMoveTo : public UPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback, const FTaskResult& PreviousTaskResult) override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere)
	float StopDist = 50;
	UPROPERTY(EditAnywhere)
	float GotoDirectDistance = 500;
	UPROPERTY(EditAnywhere)
	float OutOfSightDistance = 1000;
	
private:
	int Direction = 1;
	
	float StopDistSquared;
	float GotoDirectDistanceSquared;
	float OutOfSightDistanceSquared;

	bool bOffRadius = false;
	
	UPROPERTY()
	AActor* TargetActor = nullptr;
	UPROPERTY()
	ANpcBase* Npc = nullptr;
	UPROPERTY()
	UHTNComponent* HTNDomain = nullptr;

	FWorldStateContainer RequirementContainer;
	
	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	FVector DirToTarget;
	
	void CheckOrientationToTarget();
};
