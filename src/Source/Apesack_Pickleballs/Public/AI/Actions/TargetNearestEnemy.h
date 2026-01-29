// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TargetNearestNpc.h"

class ANpc;

class APESACK_PICKLEBALLS_API FTargetNearestEnemyAction: public FTargetNearestNpcAction
{
public:
	explicit FTargetNearestEnemyAction(ANpc* OwnerNpc);

protected:
	virtual void OnFirstExecute() override;
};

