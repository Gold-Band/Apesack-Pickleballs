// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsWidget.h"
#include "Components/VerticalBox.h"
#include "UI/GridNode.h"

void UOptionsWidget::SelectNext()
{
	if (!OptionNodes.IsEmpty()) OptionNodes[SelectedOptionIndex]->SetSelected();
	SelectedOptionIndex = (SelectedOptionIndex-1) % OptionNodes.Num();
}

void UOptionsWidget::ConfirmSelection()
{
	UE_LOG(LogTemp, Warning, TEXT("UOptionsWidget::ConfirmSelection %i"), SelectedOptionIndex)
}

/*
bool UOptionsWidget::Initialize()
{
	if (Options)
	{
		for (auto Option : Options->GetAllChildren())
		{
			UE_LOG(LogTemp, Warning, TEXT("UOptionsWidget::Initialize"))
			OptionNodes.Add(Cast<UGridNode>(Option));
		}
		SelectedOptionIndex = OptionNodes.Num()-1;
	}
	return Super::Initialize();
}
*/
