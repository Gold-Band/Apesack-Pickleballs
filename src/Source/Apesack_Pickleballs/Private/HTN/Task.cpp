// Fill out your copyright notice in the Description page of Project Settings.

#include "HTN/Task.h"

#include "HTN/HTNComponent.h"

void UPrimitiveTask::Run()
{
	bShouldTick = true;
}

void UPrimitiveTask::Pause()
{
	bShouldTick = false;
}

void UPrimitiveTask::ForceComplete()
{
	Pause();

	if (SubsystemId < 0) return;
	
	if (!Instigator) UE_LOG(LogTemp, Error, TEXT("ForceComplete() - NO INSTIGATOR"));
	UTaskSubsystem* Subsystem =  UTaskSubsystem::Get(Instigator);
	if (Subsystem && Subsystem->UnRegisterPrimitiveTask(this)) SubsystemId = -1;
}

void UPrimitiveTask::OnTaskCompleted(FTaskResult ReturnedObjects)
{
	Pause();

	if (SubsystemId < 0) return;
	
	if (UTaskSubsystem::Get(Instigator)->UnRegisterPrimitiveTask(this)) SubsystemId = -1;
	if (OnTaskCompleteCallback)	OnTaskCompleteCallback(ReturnedObjects);
}

void UPrimitiveTask::Tick(float DeltaTime)
{
	ReceiveTick(DeltaTime);
}

void UPrimitiveTask::Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback)
{
	EffectContainer = FWorldStateContainer::FromArray(Effect);
	OnTaskCompleteCallback = OnCompleteCallback;
	SubsystemId = UTaskSubsystem::Get(InstigatorActor)->RegisterPrimitiveTask(this);
	Instigator = InstigatorActor;

	if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("UPrimitiveTask::Initialize -> %s - %s"), *InstigatorActor->GetName(), *Name);

	ReceiveInitialize(InstigatorActor);
}

const FTaskResult& UPrimitiveTask::GetPreviousTaskResult() const
{
	UHTNComponent* Domain = Cast<UHTNComponent>(Instigator->GetComponentByClass(UHTNComponent::StaticClass()));
	check (Domain);
	return Domain->GetPreviousTaskResult();
}


ETickableTickType UTaskSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

bool UTaskSubsystem::IsTickableWhenPaused() const
{
	return false;
}

bool UTaskSubsystem::IsTickableInEditor() const
{
	return false;
}

int UTaskSubsystem::RegisterPrimitiveTask(UPrimitiveTask* Task)
{
	if (!Task) return -1;

	// if already registered
	const int Id = Task->GetSubsystemId();
	if (Id >= 0) return Id;

	RunningPrimitiveTasks.Add(Task);
	return IdIndex++;
}

bool UTaskSubsystem::UnRegisterPrimitiveTask(const UPrimitiveTask* Task)
{
	if (!Task) return false;
	
	const int RemoveIndex = RunningPrimitiveTasks.IndexOfByPredicate([Task](const TWeakObjectPtr<UPrimitiveTask> Elem)
	{
		return Elem->GetSubsystemId() == Task->GetSubsystemId();
	});

	RunningPrimitiveTasks.RemoveAt(RemoveIndex);
	return true;
}

void UTaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTaskSubsystem::Deinitialize()
{
	Super::Deinitialize();
	RunningPrimitiveTasks.Empty();		
}

UTaskSubsystem* UTaskSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			const UGameInstance* GameInstance = World->GetGameInstance();
			if (GameInstance) return GameInstance->GetSubsystem<UTaskSubsystem>();
		}
	}
	return nullptr;
}

void UTaskSubsystem::Tick(float DeltaTime)
{
	for (int i = 0; i < RunningPrimitiveTasks.Num(); ++i)
	{
		if (!RunningPrimitiveTasks[i].IsValid() || RunningPrimitiveTasks[i]->IsUnreachable())
		{
			RunningPrimitiveTasks.RemoveAt(i);
			i--;
			continue;
		}
		
		if (RunningPrimitiveTasks[i]->ShouldTick())
		{
			RunningPrimitiveTasks[i]->Tick(DeltaTime);
		}
	}
}

TStatId UTaskSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTaskSubsystem, STATGROUP_Tickables);
}
