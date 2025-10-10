// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldState.h"
#include "TaskResult.generated.h"

UENUM(BlueprintType)
enum class ETaskState : uint8
{
	InProgress,
	Success,
	Failed
};

USTRUCT(Blueprintable)
struct FTaskResult
{
	GENERATED_BODY()
	FTaskResult() : EndState(ETaskState::InProgress), Effect(FWorldStateContainer()), TargetActor(nullptr), PointOfInterest(FVector::Zero()),Message("") { }
	explicit FTaskResult(ETaskState State, const FWorldStateContainer& EffectResult, TObjectPtr<AActor> Target, const FString& ResultMessage): EndState(State), Effect(EffectResult), TargetActor(Target), Message(ResultMessage) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETaskState EndState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldStateContainer Effect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PointOfInterest;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;
};