// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveTask.h"
#include "UObject/NoExportTypes.h"
#include "Task.generated.h"

class UPrimitiveTask;


USTRUCT()
struct FMethod
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName Name = "Method";
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UPrimitiveTask>> Steps;
};	

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class APESACK_PICKLEBALLS_API UTask : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName Name = "Task";
	
	UPROPERTY(EditAnywhere)
	TArray<FMethod> Methods;
};
