// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTN/Task.h"
#include "GetOuterWallDefensePosition.generated.h"

class ANpcFriendly;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGetOuterWallDefensePosition : public UPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback) override;
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY()
	TObjectPtr<ANpcFriendly> InstigatorAsNpc;
	
};
