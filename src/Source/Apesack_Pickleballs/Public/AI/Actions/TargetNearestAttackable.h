// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TargetNearestNpc.h"

class UBuildingsManager;
class ANpc;

class APESACK_PICKLEBALLS_API FTargetNearestAttackableAction: public FAction
{
public:
	explicit FTargetNearestAttackableAction(ANpc* OwnerNpc);

	virtual bool IsExecutable() const override;
	virtual void Execute(float DeltaTime) override;
	virtual void Reset() override;
	
private:
	ANpc* Owner;
	bool bIsFirstCall;
	
	UWorld* World;
	FVector Location;
	
	UNpcManager* NpcManager;
	UBuildingsManager* BuildingsManager;
	
	AActor* NearestNpc;
	AActor* NearestBuilding;
	AActor* NearestAttackable;
	
	float NpcDistance;
	float BuildingDistance;
};

