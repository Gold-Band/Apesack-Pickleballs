// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/HTN/Task.h"
#include "AI/Actions/Action.h"

FTask::FTask(const FString& TaskName) : Name(TaskName)
{
	Progress = 0;
	bFailed = false;
}

bool FTask::CanPerform() const
{
	for (const auto Action : Actions)
	{
		if (Action->IsExecutable() == false) return false; 
	}
	return true;
}

void FTask::Reset()
{
	Progress = 0;
	for (const auto Action : Actions)
	{
		Action->Reset(); 
	}
}

void FTask::Run(float DeltaTime)
{
	if (!Actions.IsValidIndex(Progress)) Reset();
	
	FAction* CurrentAction = Actions[Progress];
	//UE_LOG(LogTemp, Log, TEXT("Executing \"%s\" (from %s)"), *CurrentAction->GetName(), *Name);
	CurrentAction->Execute(DeltaTime);
	
	switch (CurrentAction->State)
	{
	case EActionState::Failed:
		UE_LOG(LogTemp, Log, TEXT("Task \"%s\" Failed at \"%s\"!"), *Name, *CurrentAction->GetName());
		bFailed = true;
		break;
	case EActionState::Succeeded:
		Progress++;
	default: ;
	}
}
