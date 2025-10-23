// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsWidget.h"

#include "Buildings/Plot.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "NPC/NpcCharacter.h"
#include "UI/GridNode.h"


template void UOptionsWidget::Setup<FToolInfo>(const TArray<TOptionsData<FToolInfo>>&);
template void UOptionsWidget::Setup<FBuildingInfo>(const TArray<TOptionsData<FBuildingInfo>>&);

template <typename T>
void UOptionsWidget::Setup(const TArray<TOptionsData<T>>& Data)
{
	for (int i = 0; i < Data.Num(); ++i)
	{
		UGridNode* Node = nullptr;
		
		// if not enough nodes exist
		if (!OptionNodes.IsValidIndex(i))
		{
			Node = CreateWidget<UGridNode>(this, OptionNodeClass);
			OptionNodes.Add(Node);
		}

		// initialize node
		Node = OptionNodes[i];
		Node->Icon->SetBrushFromTexture(Data[i].Icon);
		Node->Cost = Data[i].Cost;
		Node->OrderTask = Data[i].OrderTask;
		Node->ObjectTypeInfo = static_cast<const void*>(Data[i].TypeInfo);
		if (i > 0) Node->UpNode = OptionNodes[i-1];
		if (i-1 >= 0) OptionNodes[i-1]->DownNode = Node;		
		// add node to vertical box
		Options->AddChildToVerticalBox(Node);
	}

	OptionNodes[Data.Num()-1]->DownNode = OptionNodes[0];
	OptionNodes[0]->UpNode = OptionNodes[Data.Num()-1];
}

void UOptionsWidget::Reset() const
{
	Options->ClearChildren();
}

UGridNode* UOptionsWidget::GetNode(const int Index)
{
	if (OptionNodes.IsValidIndex(Index)) return OptionNodes[Index];
	return nullptr;
}

