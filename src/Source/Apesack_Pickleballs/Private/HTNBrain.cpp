// Fill out your copyright notice in the Description page of Project Settings.
#include "HTNBrain.h"

#include "PrimitiveTask.h"
#include "Task.h"

// Sets default values for this component's properties
UHTNBrain::UHTNBrain()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0;
	// ...

}


// Called when the game starts
void UHTNBrain::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());

	UE_LOG(LogTemp, Warning, TEXT("UGoapAgent::BeginPlay"));
	
}


// Called every frame
void UHTNBrain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bRunPlan && CurrentTask)
	{
		// while in progress
		switch (CurrentTask->Run())
		{
		case ETaskCompleteType::InProgress:
			UE_LOG(LogTemp, Warning, TEXT("%s::Task in progress"), *CurrentTask->Name.ToString())
			break;
		case ETaskCompleteType::Success:
			UE_LOG(LogTemp, Warning, TEXT("%s::Task Succeeded"), *CurrentTask->Name.ToString())
			NextTask();
			break;
		case ETaskCompleteType::Failed:
			UE_LOG(LogTemp, Warning, TEXT("%s::Task Failed"),*CurrentTask->Name.ToString())
			NewPlan();
			break;
		default: ;
		}
	}

	if (bPlanCompleted)
	{
		NewPlan();
	}
}

void UHTNBrain::OnWorldStateChanged(const FWorldState& NewState)
{
	WorldState.UpdateVariables(NewState);
}

void UHTNBrain::OnWorldStateVariableChanged(const FWorldStateVariable& NewVariable)
{
	WorldState.UpdateVariable(NewVariable);
}

UHTNBrain::EPlanResult UHTNBrain::MakePlan()
{
	if (bIsPlanInProgress) return EPlanResult::InProgress;
	if (MainTasks.IsEmpty()) return EPlanResult::NoTasks;
	
	bIsPlanInProgress = true;
	
	// plannign time
	bool bIsValidTask = false;
	for (const auto& Task : MainTasks)
	{
		if (!Task) continue;

		for (const auto& Method : Task->Methods)
		{
			bool bIsValidMethod = true;
			FWorldState Hypothetical = WorldState;
			for (const auto& Step : Method.Steps)
			{
				// is valid step?
				if (!Step)
				{
					UE_LOG(LogTemp, Warning, TEXT("Detected invalid step! %s->%s"), *Task->Name.ToString(), *Method.Name.ToString())
					bIsValidMethod = false;
					break; // make method invalid
				}
				
				// can this step be performed? Is this step's requirement met?
				// compare against world state
				UE_LOG(LogTemp, Warning, TEXT("REQ:%s(%s) is %s"),*FWorldState::WorldStateOptionAsString(Step.GetDefaultObject()->Requirement.Option), Step.GetDefaultObject()->Requirement.Value?TEXT("true"):TEXT("false"), Step.GetDefaultObject()->Requirement.IsValid(Hypothetical)? TEXT("valid"):TEXT("not valid"))
				if (!Step.GetDefaultObject()->Requirement.IsValid(Hypothetical))
				{
					bIsValidMethod = false;
					break;
				}

				// At this point, step is valid
				// update hypothetical world state as if this action were to be executed successfully
				Hypothetical.UpdateVariable(Step.GetDefaultObject()->Effect);
			}
			if (bIsValidMethod)
			{
				bIsValidTask = true;
				// add every step of method to plan
				UE_LOG(LogTemp, Warning, TEXT("Valid Plan:"))
				for (uint8 i = 0; i < Method.Steps.Num(); i++)
				{
					TObjectPtr<UPrimitiveTask> CpyInstance = NewObject<UPrimitiveTask>(GetTransientPackage(), Method.Steps[i].Get());
					Plan.Enqueue(CpyInstance);
					UE_LOG(LogTemp, Warning, TEXT("%i. %s"), i+1, *CpyInstance->Name.ToString())
				}
				break;
			}
			UE_LOG(LogTemp, Warning, TEXT("Method %s is invalid!"), *Method.Name.ToString());
		}
		
		if (bIsValidTask)
		{
			bIsPlanInProgress = false;
			return EPlanResult::Success;
		}
	}

	bIsPlanInProgress = false;
	return EPlanResult::Failed;
}

void UHTNBrain::NextTask()
{
	AActor* TaskReturn = CurrentTask->ReturnedTarget;
		
	if (Plan.Dequeue(CurrentTask))
	{
		if (CurrentTask) CurrentTask->Init(OwnerPawn, TaskReturn);
		else UE_LOG(LogTemp, Warning, TEXT("Current task is null!"));
	}
	else
	{
		bRunPlan = false;
		bPlanCompleted = true;
	}
}

void UHTNBrain::NewPlan()
{
	Plan.Empty();
	CurrentTask = nullptr;
	switch(MakePlan())
	{
	case Success:
		// Start plan
		if (Plan.Dequeue(CurrentTask))
		{
			bRunPlan = true;
			bPlanCompleted = false;
			CurrentTask->Init( OwnerPawn);
		}
		else
		{
			bRunPlan = false;
			bPlanCompleted = true;
		}
		break;
	case InProgress:
		UE_LOG(LogTemp, Warning, TEXT("Plan already in progress!"));
		break;
	case NoTasks:
		UE_LOG(LogTemp, Warning, TEXT("No tasks!"));
		break;
	case Failed:
		UE_LOG(LogTemp, Warning, TEXT("Plan failed!"));
		break;
	}
}
