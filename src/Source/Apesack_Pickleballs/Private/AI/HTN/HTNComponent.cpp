// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/Task.h"

UHTNComponent::UHTNComponent()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UHTNComponent::AssignTask(FTask* Task, int Priority)
{
	if (Priority >= 0) Tasks.Insert(Task, Priority);
	else Tasks.Add(Task);
}

void UHTNComponent::UpdatePlan()
{
	bHasValidTask = false;
	
	for (const auto Task : Tasks)
	{
		if (!Task->CanPerform()) continue;
		/*if (Task->Failed())
		{
			Task->SoftReset();
			continue;
			UE_LOG(LogTemp, Warning, TEXT("Task that failed-%s"), *Task->GetName())
			if (CurrentTask && CurrentTask->Succeeded())
			{
				UE_LOG(LogTemp, Warning, TEXT("CurrentTask that succeeded-%s"), *CurrentTask->GetName())
				Task->Reset();
			}
			else continue;
		}*/
		
		CurrentTask = Task;
		bHasValidTask = true;
		break;
	}
}


void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int r = 0;
	while (bHasValidTask == false) // default is false
	{
		UpdatePlan(); // sets has valid task (to true)
		
		if (bHasValidTask)
		{
			CurrentTask->Run(DeltaTime);
			/*if (CurrentTask->Succeeded())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s Succeeded"), *CurrentTask->GetName())
			}*/
			if (CurrentTask->Failed())
			{
				CurrentTask->SoftReset(); // make it try again
			
				// but do the next action right away
				bHasValidTask = false;
				UE_LOG(LogTemp, Error, TEXT("Try the next task.. (recursions=%i)"),++r);
			}
		}
	}
}