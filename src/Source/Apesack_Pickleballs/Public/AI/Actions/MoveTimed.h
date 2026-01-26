// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"

class ANpc;

class APESACK_PICKLEBALLS_API FMoveTimedAction: public FAction
{
public:
	explicit FMoveTimedAction(ANpc* OwnerNpc);
	
	virtual bool IsExecutable() const override;
	virtual void Execute(float DeltaTime) override;
	virtual void Reset() override;
	
private:
	TObjectPtr<ANpc> Owner;
	float MoveTime;
	float ExecutionTime;
	int Direction;
};