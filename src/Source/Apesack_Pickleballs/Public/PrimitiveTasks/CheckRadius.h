// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTN/Task.h"
#include "CheckRadius.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UCheckRadius : public UPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptableMargin = 10;
	
};
