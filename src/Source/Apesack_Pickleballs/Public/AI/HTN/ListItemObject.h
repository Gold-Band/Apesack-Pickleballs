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
	
	UPROPERTY()
	const AActor* ContextActor;
	
	// function for a button
	TFunction<void()> OnActionCalledFunction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	int Cost;
	
	UPROPERTY(BlueprintReadWrite, Category="Data")
	UUserWidget* Parent;
	
	UPROPERTY(BlueprintReadWrite, Category="Data")
	mutable UUserWidget* ConstructedWidget = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category="Data")
	bool bDisable;
	
	UPROPERTY(BlueprintReadWrite, Category="Data")
	bool bCloseOnClicked = false;
};