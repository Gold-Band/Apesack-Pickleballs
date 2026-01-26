// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoPanel.generated.h"

class UListView;
class UTextBlock;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UInfoPanel : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UListView> Info;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UListView> Actions;
	
public:

	UFUNCTION(BlueprintCallable)
	void MakeActionsForObject(UObject* Object);
	
};
