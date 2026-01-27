// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"

class ANpc;

class APESACK_PICKLEBALLS_API FMoveToAction: public FAction
{
public:
	explicit FMoveToAction(ANpc* OwnerNpc);
	
	virtual bool IsExecutable() const override;
	virtual void Execute(float DeltaTime) override;
	virtual void Reset() override;

private:
	TObjectPtr<ANpc> Owner;
	AActor* TargetActor;
	TArray<FHitResult> HitResults;
	float Timer;
};