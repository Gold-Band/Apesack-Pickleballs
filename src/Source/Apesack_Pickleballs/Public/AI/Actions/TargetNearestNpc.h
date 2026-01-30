// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Managers/NpcManager.h"

class ANpc;

class APESACK_PICKLEBALLS_API FTargetNearestNpcAction: public FAction
{
public:
	explicit FTargetNearestNpcAction(ANpc* OwnerNpc);
	
	virtual bool IsExecutable() const override;
	virtual void Execute(float DeltaTime) override;
	virtual void Reset() override;
	
protected:
	virtual void OnFirstExecute();

	ENpcSearchOption NearestType;	
	TObjectPtr<ANpc> Owner;
	AActor* NearestNpc;
	bool bIsFirstCall;
};

