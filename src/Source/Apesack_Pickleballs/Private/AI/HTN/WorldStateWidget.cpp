// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/HTN/WorldStateWidget.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "Components/ListView.h"


void UWorldStateWidget::UpdateContent(const FWorldStateContainer& NewWorldState)
{
	if (WorldStateList)
	{
		ListItems.Empty();
		WorldStateList->ClearListItems();

		for (const auto& Entry : NewWorldState.Get())
		{
			UListItemObject* NewItem = NewObject<UListItemObject>();
			NewItem->DisplayText = FText::FromName(Entry.Name);
			NewItem->bState = Entry.Value;
			ListItems.Add(NewItem);
		}
		
		WorldStateList->SetListItems(ListItems);
	}
}