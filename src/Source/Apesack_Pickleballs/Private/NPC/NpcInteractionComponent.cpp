// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/NpcInteractionComponent.h"
#include "NPC/NpcCharacter.h"
#include "UI/OptionsWidget.h"

UNpcInteractionComponent::UNpcInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UNpcInteractionComponent::IsInteracting() const
{
	return SelectedNpc && SelectedNpc->AreOptionsVisible();
}

void UNpcInteractionComponent::StartInteraction()
{
	if (!SelectedNpc) return;

	SelectedNpcOptions = SelectedNpc->GetInteractionDialogue();
	SelectedNpc->OpenInteractionDialogue();
}

void UNpcInteractionComponent::EndInteraction()
{
	if (SelectedNpc && SelectedNpc->AreOptionsVisible()) SelectedNpc->CloseInteractionDialogue(); 
	SelectedNpcOptions = nullptr;
}

void UNpcInteractionComponent::SelectNpc(ANpcCharacter* OtherNpc)
{
	SelectedNpc = OtherNpc;
}

void UNpcInteractionComponent::UnSelectNpc()
{
	EndInteraction();
	SelectedNpc = nullptr;
}

void UNpcInteractionComponent::SelectNextOption()
{
	if (SelectedNpc && SelectedNpcOptions) SelectedNpcOptions->SelectNext();
}

void UNpcInteractionComponent::ConfirmSelection()
{
	if (SelectedNpc && SelectedNpcOptions) SelectedNpcOptions->ConfirmSelection();
}

void UNpcInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}
