// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FAction;

/**
 * Container of required actions to perform this task
 */

DECLARE_DELEGATE(FResetSignature)

class APESACK_PICKLEBALLS_API FTask
{
public:
	explicit FTask(const FString& TaskName);
	
	void Run(float DeltaTime);
	bool CanPerform() const;
	FString GetName() const {return Name;}
	bool Failed() const {return bFailed;}
	bool Succeeded() const {return bSuccess;}
	
	void Reset();
	void SoftReset();
	
	TArray<FAction*> Actions;
	bool bAutoReset;
	float AutoResetInterval;
	bool bPrintDebug = false;
	
private:
	bool AutoResetCondition() const;
	
	FString Name;
	int Progress;
	bool bFailed;
	bool bSuccess;
	
	float TimeSinceReset;
};