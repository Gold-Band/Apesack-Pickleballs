// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Actions/Action.h"


//class FAction;

/**
 * Container of required actions to perform this task
 */

//DECLARE_DELEGATE(FResetSignature)
//DECLARE_DELEGATE(FOnStartedSignature)
//DECLARE_DELEGATE(FOnEndedSignature)

//DECLARE_DELEGATE_RetVal(bool, FConditionCheckSignature);
//DECLARE_DELEGATE_OneParam(FExecutionFunctionSignature, float);

class AFOOLSREIGN_API FTask
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
	
	void DoCooldown(float DeltaTime);
	bool IsOnCooldown() const;
	
	//bool bAutoReset;
	//float AutoResetInterval;
	bool bPrintDebug = false;
	bool bResetOnFail = false;
	
	float Cooldown = 0;
	
	TArray<FAction> Actions;
	TFunction<bool()> Condition;
	TFunction<void()> OnStarted;
	TFunction<void()> OnEnded;
	
	void OnTaskStarted();
	
private:
	//bool AutoResetCondition() const;
	float CooldownTimer;
	
	FString Name;
	int Progress;
	
	bool bFailed;
	bool bSuccess;
	bool bTaskFirst;
	
	//float TimeSinceReset;
};