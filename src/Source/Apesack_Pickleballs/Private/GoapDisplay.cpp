// Fill out your copyright notice in the Description page of Project Settings.


#include "GoapDisplay.h"
#include "GoapSubsystem.h"
#include "ListItemObject.h"
#include "Components/ListView.h"

void UGoapDisplay::UpdateContent(const FWorldState& NewWorldState, const TArray<FWorldState>& NewGoals,
                                 const TArray<FAction>& NewActions)
{
	if (WorldStateList)
	{
		ListItems.Empty();
		WorldStateList->ClearListItems();
		//UE_LOG(LogTemp, Warning, TEXT("UGoapDisplay::UpdateContent"));

		for (const auto& Entry : NewWorldState.Variables)
		{
			UListItemObject* NewItem = NewObject<UListItemObject>();
			NewItem->DisplayText = FWorldState::WorldStateOptionAsString(Entry.Key);
			NewItem->bState = Entry.Value;
			ListItems.Add(NewItem);
		}
		
		WorldStateList->SetListItems(ListItems);
	}

	if (GoalsList)
	{
		ListItems.Empty();
		GoalsList->ClearListItems();

		for (const auto& Entry : NewGoals)
		{
			UListItemObject* NewItem = NewObject<UListItemObject>();
			NewItem->DisplayText = Entry.Name.ToString();
			ListItems.Add(NewItem);
		}
		
		GoalsList->SetListItems(ListItems);
	}

	if (ActionsList)
	{
		ListItems.Empty();
		ActionsList->ClearListItems();

		for (const auto& Entry : NewActions)
		{
			UListItemObject* NewItem = NewObject<UListItemObject>();
			NewItem->DisplayText = Entry.Name.ToString();
			ListItems.Add(NewItem);
		}
		
		ActionsList->SetListItems(ListItems);
	}
}