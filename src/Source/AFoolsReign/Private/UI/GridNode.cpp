// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GridNode.h"

#include "Components/Image.h"

void UGridNode::SetSelected()
{
	Icon->SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 1));
}

void UGridNode::SetUnselected()
{
	Icon->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
}