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
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("HTN::SetTasks")
	PreTickEvent = [&]()
	{
		Plan.Reset();
		if (CurrentTask)
		{
			CurrentTask->Pause();
			CurrentTask = nullptr;
		}

		Tasks = NewTasks;
		
		Planner = MakeUnique<FPlanner>(Tasks, &WorldStateContainer);
	};
}


void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!bTickReady)
	{
		if (++InitialTickCount > InitialFrameDelay)
		{
			bTickReady = true;
		}
		else return;
	}
	
	if (PreTickEvent)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("HTN::PreTickEvent")
		PreTickEvent();
		PreTickEvent = nullptr;
	}

	for (auto Sensor : SensorInstances)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("HTN::CheckSensor")
		if (Sensor->ShouldTick(DeltaTime))
		{
			Sensor->Tick();
		}
	}

	if ((LastPlan+=DeltaTime) >= PlanningInterval)
	{
		LastPlan-=PlanningInterval;
		
		//UE_LOG(LogTemp, Warning, TEXT("Making new plan!"))
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("HTN::TickPlan")
		if (Planner.IsValid() && Planner.Get()->RequestPlan(Plan, bLogDebug))
		{
			bGetNextTask = false;
			// start new plan
			if (CurrentTask) CurrentTask->Pause();
			CurrentTask = GetNextTaskInitialized(Plan);
			if (CurrentTask)
			{
				if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Starting plan %s"), *Plan.GetName().ToString())
				CurrentTask->Run();
			}
		}
	}

	if (bGetNextTask)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("HTN::GetNextTask")
		
		bGetNextTask = false;
		switch (Plan.LastResult.EndState)
		{
		case ETaskState::Success:
			if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Task Success"))
			// Get Next
			if (CurrentTask) CurrentTask->Pause();
			CurrentTask = nullptr;
			CurrentTask = GetNextTaskInitialized(Plan);
			if (CurrentTask) CurrentTask->Run();
			else // no tasks, make new plan
			{
				if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Completed plan!"))
				bGetNextTask = false;
			}
			break;
		case ETaskState::Failed:
			if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Task Failed! Waiting for new plan.."))
			if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("%s"), *Plan.LastResult.Message);
			Plan.Reset();
			bGetNextTask = false;
			break;
		}
	}
}

void UHTNComponent::CancelActivePlan()
{
	if (CurrentTask) CurrentTask->ForceComplete();
	CurrentTask = nullptr;
	Plan.Reset();
	if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Cancelled!"))
}

void UHTNComponent::RunTask(const TSoftObjectPtr<UTask> Task)
{
	const FPlanner TempPlanner{TArray{Task}, nullptr};
	TempPlanner.RequestPlan(Plan);
	bGetNextTask = false;
	CurrentTask = GetNextTaskInitialized(Plan);
	if (CurrentTask) CurrentTask->Run();
	if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Ordered!"))
}

void UHTNComponent::UpdateWorldState(const FString& OverrideStateName, bool OverrideValue)
{
	WorldStateContainer.SetToMatch(FWorldState(FName(OverrideStateName), OverrideValue));
}

void UHTNComponent::UpdateWorldState(const FWorldState& UpdatedWorldState)
{
	WorldStateContainer.SetToMatch(UpdatedWorldState);
}

bool UHTNComponent::VerifyWorldState(const FWorldStateContainer& VerifyContainer) const
{
	return FWorldStateContainer::HasAllMatchingCommons(WorldStateContainer, VerifyContainer, false);
}

bool UHTNComponent::IsWaiting() const
{
	if (!CurrentTask) return false;
	return CurrentTask->Name.Equals("Wait");
}

void UHTNComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Planner.IsValid())
	{
		SetTasks(Tasks);
	}
	
	InitialFrameDelay = FMath::RandRange(0, 15);
}


void UHTNComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Save a world state for each world state defined in the Domain
	if (Domain.IsValid())
	{
		TArray<FName> WorldStateNames = Domain->GetRowNames();
		TArray<FWorldStateOptions*> WorldStateValues;
		Domain->GetAllRows(TEXT("Getting all world states"), WorldStateValues);

		const auto Func = [&](const FName& Key, const FWorldStateOptions& Value)
		{
			WorldStateContainer.Add(FWorldState(Key, Value.bDefaultValue));
		};
		Domain->ForeachRow<FWorldStateOptions>(TEXT("Saving all worldstates"), Func);		
	}
	
	
	for (const auto& SensorInitializer : Sensors)
	{
		if (!SensorInitializer.SensorClass)
		{
			if (bLogDebug) UE_LOG(LogTemp, Error, TEXT("UHTNComponent::InitializeComponent  -  SensorClass is null!"))
			continue;
		}
		const int i = SensorInstances.Add(NewObject<USensor>(this, SensorInitializer.SensorClass));
		
		SensorInstances[i]->Initialize(this);
		SensorInstances[i]->TickInterval = SensorInitializer.TickInterval;
	}

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
			this->WorldStateContainer.SetToMatch(ReturnedObjects.Effect);
		};

		NextTask->Initialize(GetOwner(), Callback, Plan.LastResult);
	}
	return NextTask;
}