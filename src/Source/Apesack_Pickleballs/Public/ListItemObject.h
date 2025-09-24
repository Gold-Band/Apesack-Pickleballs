// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ListItemObject.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UListItemObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	FString DisplayText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Data")
	bool bState;
};