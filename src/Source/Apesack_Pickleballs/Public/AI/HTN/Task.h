// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FAction;

/**
 * Container of required actions to perform this task
 */
class APESACK_PICKLEBALLS_API FTask
{
public:
	explicit FTask(const FString& TaskName);
	
	void Run(float DeltaTime);
	bool CanPerform() const;
	FString GetName() const {return Name;}
	bool Failed() const {return bFailed;}
	void Reset();
	
	TArray<FAction*> Actions;
	
private:
	
	FString Name;
	int Progress;
	bool bFailed;
};