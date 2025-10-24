// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldStateWidget.generated.h"

struct FWorldStateContainer;
class UListItemObject;
struct FWorldState;
class UListView;
class UHTNBrain;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UWorldStateWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateContent(const FWorldStateContainer& NewWorldState);

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UListView> WorldStateList;

private:
	
	UPROPERTY()
	TArray<TObjectPtr<UListItemObject>> ListItems;
};
