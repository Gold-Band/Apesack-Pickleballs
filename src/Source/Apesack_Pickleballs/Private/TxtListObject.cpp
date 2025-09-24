// Fill out your copyright notice in the Description page of Project Settings.


#include "TxtListObject.h"

#include "ListItemObject.h"
#include "Components/TextBlock.h"

void UTxtListObject::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UListItemObject* ListItem = Cast<UListItemObject>(ListItemObject);
	if (!ListItem || !Text) return;

	Text->SetText(FText::FromString(ListItem->DisplayText));
}
