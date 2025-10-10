// Fill out your copyright notice in the Description page of Project Settings.

#include "HTN/WorldStateWidget.h"
#include "HTN/HTNComponent.h"
#include "HTN/ListItemObject.h"
#include "Components/ListView.h"


void UWorldStateWidget::UpdateContent(const TArray<FWorldState*>& NewWorldState)
{
	if (WorldStateList)
	{
		ListItems.Empty();
		WorldStateList->ClearListItems();

		for (const auto& Entry : NewWorldState)
		{
			UListItemObject* NewItem = NewObject<UListItemObject>();
			NewItem->DisplayText = FText::FromName(Entry->Name);
			NewItem->bState = Entry->GetValue();
			ListItems.Add(NewItem);
		}
		
		WorldStateList->SetListItems(ListItems);
	}
}