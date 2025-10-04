// Fill out your copyright notice in the Description page of Project Settings.
#include "HTN/HTNComponent.h"
#include "HTN/Task.h"

UHTNComponent::UHTNComponent()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UHTNComponent::SetTasks(const TArray<TSoftObjectPtr<UTask>>& NewTasks)
{
	PreTickEvent = [&]()
	{
		Tasks = NewTasks;
		Plan.Reset();
		if (CurrentTask)
		{
			CurrentTask->Pause();
			CurrentTask = nullptr;
		}

		Planner = MakeUnique<FPlanner>(Tasks, &WorldStateContainer);
	};
}


void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PreTickEvent)
	{
		PreTickEvent();
		PreTickEvent = nullptr;
	}

	
	for (auto Sensor : SensorInstances)
	{
		if (Sensor && Sensor->ShouldTick())
		{
			Sensor->Tick(DeltaTime);
		}
	}

	if ((LastPlan+=DeltaTime) >= PlanningInterval)
	{
		LastPlan-=PlanningInterval;
		
		//UE_LOG(LogTemp, Warning, TEXT("Making new plan!"))
		if (Planner.IsValid() && Planner.Get()->NewPlan(Plan))
		{
			bGetNextTask = false;
			// start new plan
			if (CurrentTask) CurrentTask->Pause();
			CurrentTask = GetNextTaskInitialized(Plan);
			if (CurrentTask) CurrentTask->Run();
		}
	}

	if (bGetNextTask)
	{
		bGetNextTask = false;
		switch (Plan.LastResult.EndState)
		{
		case ETaskState::InProgress:
			//UE_LOG(LogTemp, Log, TEXT("No previous tasks"))
			break;
		case ETaskState::Success:
			//UE_LOG(LogTemp, Warning, TEXT("Task Success"))
			// Get Next
			if (CurrentTask) CurrentTask->Pause();
			CurrentTask = nullptr;
			CurrentTask = GetNextTaskInitialized(Plan);
			if (CurrentTask) CurrentTask->Run();
			else // no tasks, make new plan
			{
				//UE_LOG(LogTemp, Warning, TEXT("Completed plan!"))
				bGetNextTask = false;
			}
			break;
		case ETaskState::Failed:
			//UE_LOG(LogTemp, Warning, TEXT("Task Failed! Waiting for new plan.."))
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Plan.LastResult.Message);
			Plan.Reset();
			bGetNextTask = false;
			break;
		default: ;
		}
		
	}
}

void UHTNComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHTNComponent::InitializeComponent()
{
	Super::InitializeComponent();

	TArray<FWorldState*> WorldStates;
	for (const auto& SensorInitializer : Sensors)
	{
		if (!SensorInitializer.SensorClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UHTNComponent::InitializeComponent  -  SensorClass is null!"))
			continue;
		}
		const int i = SensorInstances.Add(NewObject<USensor>(this, SensorInitializer.SensorClass.Get(), EName::None, RF_NoFlags, SensorInitializer.SensorClass.GetDefaultObject()));
		//SensorInstances[i]->Initialize(GetOwner(), );
		
		auto Callback = [&](const FWorldState& SensedWorldState)
		{
			WorldStateContainer.SetToMatch(SensedWorldState);
		};
		SensorInstances[i]->Initialize(GetOwner(), Callback);
		WorldStates.Add(&SensorInstances[i]->WorldState);
		SensorInstances[i]->TickInterval = SensorInitializer.TickInterval;
	}
	WorldStateContainer = FWorldStateContainer::FromArray(WorldStates);
	Planner = MakeUnique<FPlanner>(Tasks, &WorldStateContainer);
}

TObjectPtr<UPrimitiveTask> UHTNComponent::GetNextTaskInitialized(FHTNPlan& InPlan)
{
	TObjectPtr<UPrimitiveTask> NextTask;
	TSubclassOf<UPrimitiveTask> NextTaskClass;
	if (InPlan.Dequeue(NextTaskClass))
	{
		NextTask = NewObject<UPrimitiveTask>(this, NextTaskClass.Get());

		// task finished callback
		auto Callback = [&](const FTaskResult& ReturnedObjects)
		{
			InPlan.LastResult = ReturnedObjects;
			bGetNextTask = true;
			WorldStateContainer.SetToMatch(ReturnedObjects.Effect);
		};

		NextTask->Initialize(GetOwner(), Callback);
	}
	return NextTask;
}