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
	TArray<FWorldStateMaker> Buffer;
	for (const auto& Task: Tasks)
	{
		if (!Task.LoadSynchronous()) continue;
		for (const auto& Method : Task.LoadSynchronous()->Methods)
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

bool FPlanner::NewPlan(FHTNPlan& OutPlan) const
{
	FHTNPlan NewPlan;
	switch(MakePlan(NewPlan))
	{
	case Success:
		if (NewPlan.Priority < OutPlan.Priority)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Plan success!"))
			OutPlan = NewPlan;
			return true;
		}
	case LowerPriority:
		//UE_LOG(LogTemp, Warning, TEXT("Priority not high enough!"))
		break;
	case InProgress:
		//UE_LOG(LogTemp, Warning, TEXT("Plan already in progress!"));
		break;
	case NoTasks:
		//UE_LOG(LogTemp, Warning, TEXT("No tasks!"));
		break;
	case Failed:
		//UE_LOG(LogTemp, Warning, TEXT("Plan failed!"));
		break;
	default: ;
	}
	return false;
}

FPlanner::EPlanResult FPlanner::MakePlan(FHTNPlan& OutPlan) const
{
	if (Tasks.IsEmpty()) return EPlanResult::NoTasks;
	
	// plannign time
	bool bIsValidTask = false;
	OutPlan.Priority = -1;
	for (const auto& Task : Tasks) // highest priority == 0
	{
		OutPlan.Priority++;
		if (!Task) continue;

		for (const auto& Method : Task->Methods)
		{
			bool bIsValidMethod = true;

			FWorldStateContainer Hypothetical = *WorldStates;
			Hypothetical.MergeUnique(TasksStates);

			for (const auto& Step : Method.Steps)
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
				const FWorldStateContainer Req = FWorldStateContainer::FromArray(Step.GetDefaultObject()->Requirement);
				if (Req != Hypothetical)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Requirement not met! %s->%s->%s"), *Task->Name.ToString(), *Method.Name.ToString(), *Step.GetDefaultObject()->Name)
					bIsValidMethod = false;
					break;
				}

				// At this point, step is valid
				// update hypothetical world state as if this action were to be executed successfully
				const FWorldStateContainer Eff = FWorldStateContainer::FromArray(Step.GetDefaultObject()->Effect);
				Hypothetical.SetToMatch(Eff);
			}
			if (bIsValidMethod)
			{
				bIsValidTask = true;
				// add every step of method to plan
				//UE_LOG(LogTemp, Warning, TEXT("Valid Plan:"))
				for (uint8 i = 0; i < Method.Steps.Num(); i++)
				{
					OutPlan.Add(Method.Steps[i]);
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