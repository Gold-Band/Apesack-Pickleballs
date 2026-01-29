// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/HTN/Task.h"
#include "AI/Actions/Action.h"

FTask::FTask(const FString& TaskName) : Name(TaskName)
{
	Progress = 0;
	bFailed = false;
	bAutoReset = false;
	AutoResetInterval = 0.2f;
	TimeSinceReset = 0;
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
	TimeSinceReset = 0;
	bFailed=false;
	Progress = 0;
	for (const auto Action : Actions)
	{
		Action->Reset(); 
	}
}

// returns true if we want to reset
bool FTask::AutoResetCondition() const
{
	return bAutoReset == true && TimeSinceReset >= AutoResetInterval;	
}

void FTask::Run(float DeltaTime)
{
	if (!Actions.IsValidIndex(Progress) || AutoResetCondition()) Reset();
	
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
	
	if (bAutoReset) TimeSinceReset += DeltaTime;
}
