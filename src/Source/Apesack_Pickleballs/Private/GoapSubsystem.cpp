// Fill out your copyright notice in the Description page of Project Settings.


#include "GoapSubsystem.h"

FString FWorldState::ToString() const 
{
	FString String;
	for (const TPair<EWorldStateOption, bool>& Variable : Variables)
	{
		switch (Variable.Key)
		{
		case EWorldStateOption::CanMove:
			String.Append(FString("CanMove"));
			break;
		case EWorldStateOption::CanAttack:
			String.Append(FString("CanAttack"));
			break;
		case EWorldStateOption::HasTarget:
			String.Append(FString("HasTarget"));
			break;
		case EWorldStateOption::CanPickup:
			String.Append(FString("CanPickup"));
			break;
		case EWorldStateOption::IsAfraid:
			String.Append(FString("IsAfraid"));
			break;
		case EWorldStateOption::IsEnabled:
			String.Append(FString("IsEnabled"));
			break;
		case EWorldStateOption::IsTargetAlive:
			String.Append(FString("IsTargetAlive"));
			break;
		case EWorldStateOption::IsSafeFromDanger:
			String.Append(FString("IsSafeFromDanger"));
			break;
		case EWorldStateOption::IsArmed:
			String.Append(FString("IsArmed"));
			break;
		case EWorldStateOption::WantsAnObject:
			String.Append(FString("WantsAnObject"));
			break;
		case EWorldStateOption::WantsToWander:
			String.Append(FString("WantsToWander"));
			break;
		case EWorldStateOption::IsWorking:
			String.Append(FString("IsWorking"));
			break;
			default:
		return FString("Unknown");
		}
		String.Append(FString::Printf(L": %s\n", Variable.Value ? L"True" : L"False"));
	}
	return String;
}

void UGoapSubsystem::RequestPlan(FWorldState WorldState, FWorldState GoalState, TArray<FAction>& Actions)
{
	
}

void UGoapSubsystem::GeneratePlan(FWorldState WorldState, FWorldState GoalState, const TArray<FAction>& Actions)
{
	TArray<FAction> AvailableActions = Actions;
	TArray<FAction> NewPlan;

	while (!NewPlan.IsEmpty() && NewPlan.Top().Requirement == WorldState)
	{
		
		// is there an action that can satisfy the goal?
		TArray<FAction> ValidActions;
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
			if (NewPlan.IsEmpty()) Result.Message = FString::Printf(L"No usable actions to satisfy goal state \"%s\"", *GoalState.ToString());
			else Result.Message = FString::Printf(L"No actions can satisfy goal state \"%s\"", *GoalState.ToString());
			if (OnPlanCompleted.IsBound()) OnPlanCompleted.Broadcast(Result);
			return;
		}
		
		FAction ActionUsed;
		ActionUsed.Cost = MAX_uint8;
		for (int i = 0; i < ValidActions.Num(); i++)
		{
			if (ValidActions[i].Cost < ActionUsed.Cost) ActionUsed = ValidActions[i];
		}

		NewPlan.Add(ActionUsed);
	}

	// factor in heuristics
}
