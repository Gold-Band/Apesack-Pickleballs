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
	case EWorldStateOption::None:
		return TEXT("None");
	case EWorldStateOption::CanMove:
		return FString("CanMove");
	case EWorldStateOption::CanAttack:
		return FString("CanAttack");
	case EWorldStateOption::CanPickup:
		return FString("CanPickup");
	case EWorldStateOption::HasTarget:
		return FString("HasTarget");
	case EWorldStateOption::CanBuild:
		return FString("CanBuild");
	case EWorldStateOption::CoinsExist:
		return FString("CoinsExist");
	case EWorldStateOption::ToolsExist:
		return FString("ToolsExist");
	case EWorldStateOption::AtTarget:
		return FString("AtTarget");
	default:
		return FString("Unknown");
	}
}

void FWorldState::UpdateVariable(const FWorldStateVariable& NewWorldStateVariable)
{
	if (!Variables.Contains(NewWorldStateVariable.Option)) return;
	Variables[NewWorldStateVariable.Option] = NewWorldStateVariable.Value;
}

void FWorldState::UpdateVariables(const FWorldState& NewWorldState)
{
	for (auto& Pair : NewWorldState.Variables)
	{
		if (Variables.Contains(Pair.Key))
		{
			Variables[Pair.Key] = Pair.Value;
		}
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
