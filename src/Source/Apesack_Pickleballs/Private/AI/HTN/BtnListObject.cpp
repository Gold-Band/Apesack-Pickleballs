// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/HTN/BtnListObject.h"
#include "AI/HTN/ListItemObject.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBtnListObject::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);
	
	const UListItemObject* ListItem = Cast<UListItemObject>(ListItemObject);
	if (!ListItem || !Text) return;

	const FString Txt = FString::Printf(TEXT("%s | Cost: %i"), *ListItem->DisplayText.ToString(), ListItem->Cost);
	Text->SetText(FText::FromString(Txt));
	
	//Action->OnClicked.AddDynamic(ListItemObject->ContextActor, &ListItemObject->OnActionCalledFunction);
}
