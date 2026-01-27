// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListItemObject.generated.h"

class APlot;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class APESACK_PICKLEBALLS_API UListItemObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	FText DisplayText;
	
	// function for a button
	UPROPERTY()
	const AActor* ContextActor;
	
	TFunction<void()> OnActionCalledFunction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	int Cost;
};