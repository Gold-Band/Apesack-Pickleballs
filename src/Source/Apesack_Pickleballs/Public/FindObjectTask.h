// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveTask.h"
#include "FindObjectTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class APESACK_PICKLEBALLS_API UFindObjectTask : public UPrimitiveTask
{
	GENERATED_BODY()

public:
	virtual void Init(AActor* OwningActorRef, AActor* TargetRef = nullptr) override;
	virtual ETaskCompleteType Run() override;
protected:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> FindActorsWithTag(const FName& Tag) const;
	
};
