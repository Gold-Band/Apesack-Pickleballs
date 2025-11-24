#include "HTN/Planner.h"
#include "HTN/Task.h"
bool FHTNPlan::Dequeue(TSubclassOf<UPrimitiveTask>& OutElem)
{
	const bool bResult = Plan.TryPopLast(OutElem);
	if (!bResult) Priority = MAX_int8;
	return bResult;
}


FPlanner::FPlanner(const TArray<TSoftObjectPtr<UTask>>& InTasks, const FWorldStateContainer* InWorldStates):
	Tasks(InTasks), WorldStates(InWorldStates)
{
	if (InWorldStates == nullptr) return;
	
	TArray<FWorldStateMaker> Buffer;
	for (const auto& Task: Tasks)
	{
		UTask* T = Task.LoadSynchronous();
		check(T);
		for (const auto& Method : T->Methods)
		{
			for (const auto& Step : Method.Steps)
			{
				if (!Step) continue;
				Buffer.Append(Step.GetDefaultObject()->Requirement);
				Buffer.Append(Step.GetDefaultObject()->Effect);
			}
		}
	}
	TasksStates.MergeUnique(FWorldStateContainer::FromArray(Buffer));
}

bool FPlanner::NewPlan(FHTNPlan& OutPlan, bool bLogDebug) const
{
	FHTNPlan NewPlan;
	switch(MakePlan(NewPlan))
	{
	case Success:
		if (NewPlan.Priority < OutPlan.Priority)
		{
			if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Plan success!"))
			OutPlan = NewPlan;
			return true;
		}
	case LowerPriority:
		if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Priority not high enough!"))
		break;
	case InProgress:
		if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Plan already in progress!"));
		break;
	case NoTasks:
		if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("No tasks!"));
		break;
	case Failed:
		if (bLogDebug) UE_LOG(LogTemp, Warning, TEXT("Plan failed!"));
		break;
	default: ;
	}
	return false;
}

FPlanner::EPlanResult FPlanner::MakePlan(FHTNPlan& OutPlan) const
{
	if (Tasks.IsEmpty()) return EPlanResult::NoTasks;
	
	bool bIsValidTask = false;
	OutPlan.Priority = -1;
	
	// for each high level task (data asset) available to us...
	for (auto Task : Tasks) // highest priority == 0
	{
		OutPlan.Priority++;
		
		// is this even valid?
		auto T = Task.LoadSynchronous();
		if (!T)
		{
			UE_LOG(LogTemp, Error, TEXT("Planner.cpp(73): Task is null!!"))
			continue;
		}
		
		// for each method in this high level task...
		for (const auto& Method : T->Methods) //
		{
			bool bIsValidMethod = true;

			FWorldStateContainer Hypothetical = WorldStates? *WorldStates: FWorldStateContainer();
			if (WorldStates) Hypothetical.MergeUnique(TasksStates);

			for (auto Step : Method.Steps)
			{
				// is valid step?
				if (!Step)
				{
					UE_LOG(LogTemp, Warning, TEXT("Detected invalid step! %s->%s"), *Task->Name.ToString(), *Method.Name.ToString())
					bIsValidMethod = false;
					break; // make method invalid
				}
				
				// can this step be performed? Is this step's requirement met?
				// compare against world state
				if (WorldStates)
				{
					const FWorldStateContainer Req = FWorldStateContainer::FromArray(Step.GetDefaultObject()->Requirement);
					if (Req != Hypothetical)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Requirement not met! %s->%s->%s"), *Task->Name.ToString(), *Method.Name.ToString(), *Step.GetDefaultObject()->Name)
						bIsValidMethod = false;
						break;
					}
				}

				// At this point, step is valid
				// update hypothetical world state as if this action were to be executed successfully
				if (WorldStates)
				{
					const FWorldStateContainer Eff = FWorldStateContainer::FromArray(Step.GetDefaultObject()->Effect);
					Hypothetical.SetToMatch(Eff);
				}
			}
			if (bIsValidMethod)
			{
				bIsValidTask = true;
				// add every step of method to plan
				//UE_LOG(LogTemp, Warning, TEXT("Valid Plan:"))
				for (uint8 i = 0; i < Method.Steps.Num(); i++)
				{
					OutPlan.Add(Method.Steps[i]);
					OutPlan.PlanName = FText::FromName(Method.Name);
					//UE_LOG(LogTemp, Warning, TEXT("%i. %s"), i+1, *Method.Steps[i].GetDefaultObject()->Name)
				}
				break;
			}
			//UE_LOG(LogTemp, Warning, TEXT("Method %s is invalid!"), *Method.Name.ToString());
		}
		
		if (bIsValidTask)
		{
			return EPlanResult::Success;
		}
	}

	return EPlanResult::Failed;
}