// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoapDisplay.generated.h"

class UListItemObject;
struct FAction;
struct FWorldState;
class UListView;
class UHTNBrain;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGoapDisplay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateContent(const FWorldState& NewWorldState, const TArray<FWorldState>& NewGoals, const TArray<FAction>& NewActions);

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UListView> WorldStateList;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UListView> GoalsList;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UListView> ActionsList;

private:
	
	UPROPERTY()
	TArray<UListItemObject*> ListItems;
};
