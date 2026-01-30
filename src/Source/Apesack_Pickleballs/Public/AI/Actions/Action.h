// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EActionState : uint8
{
	InProgress,
	Succeeded,
	Failed
};

DECLARE_DELEGATE_OneParam(FExecutionFunctionSignature, float);
DECLARE_DELEGATE_RetVal(bool, FConditionCheckSignature);
DECLARE_DELEGATE(FResetSignature);

class APESACK_PICKLEBALLS_API FAction // does one little thing
{

public:
	explicit FAction(const FString& ActionName);
	~FAction();
	
	void Reset();
	bool CanExecute() const;

	FString GetName() const {return Name;}
	EActionState State;
	
	FExecutionFunctionSignature ExecutionDelegate;
	FConditionCheckSignature ConditionDelegate;
	FResetSignature ResetDelegate;
	
private:
	FString Name = "Action Base";
};