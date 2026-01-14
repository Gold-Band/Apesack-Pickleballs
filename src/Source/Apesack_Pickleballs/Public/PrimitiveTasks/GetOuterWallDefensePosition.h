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
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin=1, UIMax=3))
	int Zone = 1;
	
private:
	UPROPERTY()
	TObjectPtr<ANpcFriendly> InstigatorAsNpc;
	
};
