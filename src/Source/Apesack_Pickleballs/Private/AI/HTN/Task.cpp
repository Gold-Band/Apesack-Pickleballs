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
	bTaskFirst = true;
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
	bTaskFirst = true;
}

void FTask::OnTaskStarted()
{
	OnStartedDelegate.ExecuteIfBound();
	bTaskFirst = false;
}

// returns true if we want to reset
bool FTask::AutoResetCondition() const
{
	return bAutoReset == true && TimeSinceReset >= AutoResetInterval;	
}

void FTask::Run(float DeltaTime)
{
	if (bTaskFirst) OnTaskStarted();
	
	if (AutoResetCondition()) Reset();
	
	if (!Actions.IsValidIndex(Progress))
	{
		Reset();
		OnEndedDelegate.ExecuteIfBound();
	}
	
	FAction* CurrentAction = Actions[Progress];
	
#if WITH_EDITOR
	if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Executing \"%s\" (from %s)"), *CurrentAction->GetName(), *Name);
#endif
	
	CurrentAction->ExecutionDelegate.Execute(DeltaTime);
	
	switch (CurrentAction->State)
	{
	case EActionState::Failed:
#if WITH_EDITOR
		if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Task \"%s\" Failed at \"%s\"!"), *Name, *CurrentAction->GetName());
#endif
		bFailed = true;
		break;
	case EActionState::Succeeded:
		Progress++;
		bSuccess = true;
	default:; 
	}
	
	if (bAutoReset) TimeSinceReset += DeltaTime;
}
