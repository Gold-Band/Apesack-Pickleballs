// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/Task.h"

UHTNComponent::UHTNComponent()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UHTNComponent::AssignTask(const FTask& Task, int Priority)
{
	Tasks.Insert(Task, Priority);
}

void UHTNComponent::UpdatePlan()
{
	bHasValidTask = false;
	
	for (auto& Task : Tasks)
	{
		if (!Task.CanPerform()) continue;
		
		CurrentTask = &Task;
		bHasValidTask = true;
		break;
	}
}


void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePlan();
	
	if (bHasValidTask)
	{
		CurrentTask->Run(DeltaTime);
		if (CurrentTask->Failed())
		{
			
		}
	}
}