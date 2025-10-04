// Fill out your copyright notice in the Description page of Project Settings.

#include "HTN/TxtListObject.h"
#include "HTN/ListItemObject.h"
#include "Components/TextBlock.h"

void UTxtListObject::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UListItemObject* ListItem = Cast<UListItemObject>(ListItemObject);
	if (!ListItem || !Text) return;

	Text->SetText(ListItem->DisplayText);
}
