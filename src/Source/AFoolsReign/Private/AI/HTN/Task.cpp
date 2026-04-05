// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/HTN/Task.h"
#include "AI/Actions/Action.h"

FTask::FTask(const FString& TaskName) : Name(TaskName)
{
	Progress = 0;
	bFailed = false;
	bSuccess = false;
	CooldownTimer = Cooldown;
	bTaskFirst = true;
}

bool FTask::CanPerform() const
{
	const bool bIsOnCooldown = IsOnCooldown();
	
	if (!Condition) return !bIsOnCooldown;
	return Condition() && !bIsOnCooldown;
}

void FTask::Reset()
{
	bFailed=false;
	bSuccess=false;
	bTaskFirst = true;
	CooldownTimer = 0.0f;
	Progress=0;
}

void FTask::DoCooldown(float DeltaTime)
{
	CooldownTimer += DeltaTime;
}

bool FTask::IsOnCooldown() const
{
	return CooldownTimer < Cooldown;
}

void FTask::OnTaskStarted()
{
	bTaskFirst = false;
	if (OnStarted) OnStarted();
}

void FTask::Run(float DeltaTime)
{
	if (!Actions.IsValidIndex(Progress))
	{
		Reset();
		if (OnEnded) OnEnded();
	}
	
	auto* CurrentAction = &Actions[Progress];
	const auto& Func = CurrentAction->Func;
	
	
#if WITH_EDITOR
	if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Executing \"%s\" (from %s)"), *CurrentAction->GetName(), *Name);
#endif
	
	if (bTaskFirst) OnTaskStarted();
	
	if (!Func)
	{
#if WITH_EDITOR
		if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Task \"%s\" Failed at \"%s\" (invalid func)!"), *Name, *CurrentAction->GetName());
#endif
		if (OnFailed) OnFailed();
		bFailed = true;
		return;
	}
	
	switch (Func(DeltaTime))
	{
	case EActionState::Failed:
#if WITH_EDITOR
		if (bPrintDebug) UE_LOG(LogTemp, Log, TEXT("Task \"%s\" Failed at \"%s\"!"), *Name, *CurrentAction->GetName());
#endif
		if (OnFailed) OnFailed();
		bFailed = true;
		break;
	case EActionState::Succeeded:
		Progress++;
		bSuccess = true;
	default:; 
	}
}
