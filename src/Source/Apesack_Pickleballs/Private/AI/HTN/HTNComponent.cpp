// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/Task.h"

UHTNComponent::UHTNComponent()
{
	bWantsInitializeComponent = false;
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UHTNComponent::AssignTask(FTask* Task, int Priority)
{
	if (Priority >= 0) Tasks.Insert(Task, Priority);
	else Tasks.Add(Task);
}

bool UHTNComponent::UpdatePlan()
{
	for (const auto Task : Tasks)
	{
		if (!Task->CanPerform()) continue;
		if (Task->Failed())
		{
			if (Task->bResetOnFail) Task->Reset();
			else Task->SoftReset();
			continue;
		}
		
		CurrentTask = Task;
		return true;
	}
	
	return false;
}


void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int r = 0;
	bool bHasValidTask = false;
	if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT(""));
	while (bHasValidTask == false) // default is false
	{
		bHasValidTask = UpdatePlan(); // sets has valid task (to true)
		
		if (bHasValidTask)
		{
			CurrentTask->bPrintDebug = bPrintDebug;
			CurrentTask->Run(DeltaTime);
			if (CurrentTask->Failed())
			{
				// but do the next action right away
				bHasValidTask = false;
				if (bPrintDebug) UE_LOG(LogTemp, Error, TEXT("Try the next task.. (recursions=%i)"),++r);
			}
		}
	}
}