// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <locale>

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GoapSubsystem.generated.h"


class UGoapAgent;

UENUM()
enum class EWorldStateOption : uint8
{
	CanMove,
	CanAttack,
	HasTarget,
	CanPickup,
	IsAfraid,
	IsEnabled,
	IsTargetAlive,
	IsSafeFromDanger,
	IsArmed,	
	WantsAnObject,
	WantsToWander,
	IsWorking,
	IsEquipped
};


USTRUCT(BlueprintType)
struct FWorldState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Name;
	
	UPROPERTY(EditAnywhere)
	TMap<EWorldStateOption, bool> Variables;
	
	bool operator == (const FWorldState& Other) const {
		const TMap<EWorldStateOption, bool>& OtherVariables = Other.Variables;
		for (const TPair<EWorldStateOption, bool>& Element : Variables)
		{
			if (OtherVariables.Contains(Element.Key) && OtherVariables[Element.Key] != Element.Value) return false;
		}
		return true;
	}

	FString ToString() const;
	static FString WorldStateOptionAsString(const EWorldStateOption& WorldStateOption);
};

/**
 *	FAction
 *	Represents an action that can be performed by the agent that promises to change a WorldState
 */
USTRUCT(BlueprintType)
struct FAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	FWorldState Effect;

	UPROPERTY(EditAnywhere)
	FWorldState Requirement;

	UPROPERTY(EditAnywhere)
	uint8 Cost = MAX_uint8;
};

/**
 * FPlanResult
 * Structure containing information relevant to the result of creating a goap plan.
 */
USTRUCT(BlueprintType)
struct FPlanResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly)
	FString Message;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FAction> Plan;

	UPROPERTY(BlueprintReadOnly)
	UGoapAgent* Agent;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlanCompletedDelegate, FPlanResult, PlanResult);


/**
 * UGoapSubsystem
 * A level's goap planner for all AGoapAgents 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGoapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	/** Each requests gets a priority, and placed in a queue to be worked on asynchronously */
	//UFUNCTION(BlueprintCallable)
	static void RequestPlan(FWorldState WorldState, FWorldState GoalState, TArray<FAction> Actions, TFunction<void(const FPlanResult&)> OnPlanCompletedCallback);

	UPROPERTY(BlueprintAssignable)
	FOnPlanCompletedDelegate OnPlanCompleted;
	
private:
	
	struct FRequest
	{
		FWorldState WorldState;
		FWorldState GoalState;
		TArray<FAction> Actions;
	};

	static void GeneratePlan(FWorldState WorldState, FWorldState GoalState, TArray<FAction> AvailableActions, TFunction<void(const FPlanResult&)> OnPlanCompletedCallback);
};
