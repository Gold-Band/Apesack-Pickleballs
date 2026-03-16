// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EActionState : uint8
{
	InProgress,
	Succeeded,
	Failed
};


struct AFOOLSREIGN_API FAction // does one little thing
{
	explicit FAction(const FString& ActionName) : Name(ActionName) {}
	
	FString GetName() const {return Name;}
	TFunction<EActionState(float)> Func;
	
private:
	FString Name = "Action Base";
};