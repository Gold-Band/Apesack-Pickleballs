// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "NpcHostile.generated.h"


class UNpcManager;

UCLASS()
class APESACK_PICKLEBALLS_API ANpcHostile : public ANpc
{
	GENERATED_BODY()

public:
	ANpcHostile();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	
};