// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EActionState : uint8
{
	InProgress,
	Succeeded,
	Failed
};

class APESACK_PICKLEBALLS_API FAction // does one little thing
{

public:
	virtual ~FAction() {}
	
	virtual bool IsExecutable() const {return true;} // condition - check the world state dependency (NOT the result of a dependent action) 
	virtual void Execute(float DeltaTime) = 0; // function that does or calls the one thing on owner
	virtual void Reset() { State = EActionState::InProgress; }
	
	EActionState State;
	bool bPrintDebug = false;
	
	FString GetName() const {return Name;}
	
protected:
	FString Name = "Action Base";
};