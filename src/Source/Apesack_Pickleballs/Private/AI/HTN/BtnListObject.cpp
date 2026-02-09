// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/HTN/BtnListObject.h"
#include "AI/HTN/ListItemObject.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/InfoPanel.h"

void UBtnListObject::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);
	
	const UListItemObject* ListItem = Cast<UListItemObject>(ListItemObject);
	if (!ListItem || !Text) return;

	if (ListItem->Cost > 0)
	{
		const FString Txt = FString::Printf(TEXT("%s | Cost: %i"), *ListItem->DisplayText.ToString(), ListItem->Cost);
		Text->SetText(FText::FromString(Txt));
	}
	else
	{
		const FString Txt = FString::Printf(TEXT("%s"), *ListItem->DisplayText.ToString());
		Text->SetText(FText::FromString(Txt));
	}
	
	ActionBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
	ButtonFunction = ListItem->OnActionCalledFunction;
	Parent = ListItem->Parent;
	Cost = ListItem->Cost;
	
	ActionBtn->SetIsEnabled(!ListItem->bDisable);
}

void UBtnListObject::OnButtonClicked()
{
	check(Parent);
	UInfoPanel* InfoPanel = Cast<UInfoPanel>(Parent);
	InfoPanel->OnActionEntryClicked(Cost);
	if (InfoPanel->bTransactionSucceeded)
	{
		ButtonFunction.CheckCallable();
		ButtonFunction();
	}
}
