// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/HTN/Task.h"
#include "AI/Actions/Action.h"

FTask::FTask(const FString& TaskName) : Name(TaskName)
{
	Progress = 0;
	bFailed = false;
	bSuccess = false;
	bAutoReset = false;
	AutoResetInterval = 0.2f;
	TimeSinceReset = 0;
}

bool FTask::CanPerform() const
{
	for (const auto Action : Actions)
	{
		if (Action->CanExecute() == false) return false; 
	}
	return true;
}

void FTask::Reset()
{
	SoftReset();
	TimeSinceReset = 0;
	Progress=0;
	for (const auto Action : Actions)
	{
		Action->Reset(); 
	}
}

void FTask::SoftReset()
{
	bFailed=false;
	bSuccess=false;
}

// returns true if we want to reset
bool FTask::AutoResetCondition() const
{
	return bAutoReset == true && TimeSinceReset >= AutoResetInterval;	
}

void FTask::Run(float DeltaTime)
{
	if (!Actions.IsValidIndex(Progress)) Reset();
	
	FAction* CurrentAction = Actions[Progress];
	if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Executing \"%s\" (from %s)"), *CurrentAction->GetName(), *Name);
	CurrentAction->ExecutionDelegate.Execute(DeltaTime);
	
	switch (CurrentAction->State)
	{
	case EActionState::Failed:
		if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Task \"%s\" Failed at \"%s\"!"), *Name, *CurrentAction->GetName());
		bFailed = true;
		break;
	case EActionState::Succeeded:
		Progress++;
		bSuccess = true;
	default: ;
	}
	
	//if (Progress == Actions.Num() && !bFailed) bSuccess = true;
	
	if (bAutoReset) TimeSinceReset += DeltaTime;
}
