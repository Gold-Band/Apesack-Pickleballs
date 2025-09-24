// Fill out your copyright notice in the Description page of Project Settings.


#include "GoapSubsystem.h"

FString FWorldState::ToString() const 
{
	FString String;
	for (const TPair<EWorldStateOption, bool>& Variable : Variables)
	{
		String.Append(WorldStateOptionAsString(Variable.Key));
		String.Append(FString::Printf(L": %s\n", Variable.Value ? L"True" : L"False"));
	}
	return String;
}

FString FWorldState::WorldStateOptionAsString(const EWorldStateOption& WorldStateOption)
{
	switch (WorldStateOption)
	{
	case EWorldStateOption::CanMove:
		return FString("CanMove");
	case EWorldStateOption::CanAttack:
		return FString("CanAttack");
	case EWorldStateOption::HasTarget:
		return FString("HasTarget");
	case EWorldStateOption::CanPickup:
		return FString("CanPickup");
	case EWorldStateOption::IsAfraid:
		return FString("IsAfraid");
	case EWorldStateOption::IsEnabled:
		return FString("IsEnabled");
	case EWorldStateOption::IsTargetAlive:
		return FString("IsTargetAlive");
	case EWorldStateOption::IsSafeFromDanger:
		return FString("IsSafeFromDanger");
	case EWorldStateOption::IsArmed:
		return FString("IsArmed");
	case EWorldStateOption::WantsAnObject:
		return FString("WantsAnObject");
	case EWorldStateOption::WantsToWander:
		return FString("WantsToWander");
	case EWorldStateOption::IsWorking:
		return FString("IsWorking");
	case EWorldStateOption::IsEquipped:
		return FString("IsEquipped");
	default:
		return FString("Unknown");
	}
}

bool UGoapSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UGoapSubsystem::RequestPlan(FWorldState WorldState, FWorldState GoalState, TArray<FAction> Actions, TFunction<void(const FPlanResult&)> OnPlanCompletedCallback)
{
	GeneratePlan(WorldState, GoalState, Actions, OnPlanCompletedCallback);
}

void UGoapSubsystem::GeneratePlan(FWorldState WorldState, FWorldState GoalState, TArray<FAction> AvailableActions, TFunction<void(const FPlanResult&)> OnPlanCompletedCallback)
{

	TArray<FAction> NewPlan = {};
	//UE_LOG(LogTemp, Log, TEXT("availableactions.count = %i"), AvailableActions.Num());
	//UE_LOG(LogTemp, Log, TEXT("newplan.count = %i"), NewPlan.Num());

	bool bSucess = false;
	while (!bSucess)
	{
		
		// is there an action that can satisfy the goal?
		TArray<FAction> ValidActions = {};
		for (int i = 0; i < AvailableActions.Num(); i++)
		{
			if (AvailableActions[i].Effect == GoalState)
			{
				ValidActions.Add(AvailableActions[i]);
				AvailableActions.RemoveAt(i);
			}
		}

		if (ValidActions.IsEmpty())
		{
			FPlanResult Result;
			Result.Plan = NewPlan;
			Result.bSuccess = false;
			if (NewPlan.IsEmpty()) Result.Message = FString::Printf(TEXT("No usable actions to satisfy goal state \"%s\""), *GoalState.ToString());
			else Result.Message = FString::Printf(TEXT("Can't complete plan for goal state \"%s\""), *GoalState.ToString());
			OnPlanCompletedCallback.CheckCallable();
			OnPlanCompletedCallback(Result);
			return;
		}
		
		FAction ActionUsed;
		for (int i = 0; i < ValidActions.Num(); i++)
		{
			if (ValidActions[i].Cost < ActionUsed.Cost) ActionUsed = ValidActions[i];
		}

		NewPlan.Add(ActionUsed);

		if (!NewPlan.IsEmpty() && NewPlan.Last().Requirement == WorldState) bSucess = true;
	}

	FPlanResult Result;
	Result.Plan = NewPlan;
	Result.bSuccess = true;
	Result.Message = FString("");
	OnPlanCompletedCallback.CheckCallable();
	OnPlanCompletedCallback(Result);
	// factor in heuristics
}
