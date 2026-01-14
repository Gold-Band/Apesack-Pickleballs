#pragma once

#include "WorldState.h"
#include "TaskResult.h"
#include "Containers/Deque.h"

class UTask;
class UPrimitiveTask;

// Wrapper for a TDeque. Will automatically set its priority to 255 when Dequeue returns false (its empty)
struct FHTNPlan
{
public:
	
	FORCEINLINE void Add(const TSubclassOf<UPrimitiveTask>& Elem) { Plan.PushFirst(Elem); }
	FORCEINLINE void Reset() { Plan.Empty(); Priority = MAX_int8; }
	FORCEINLINE const TSubclassOf<UPrimitiveTask>& Peek() const { return Plan.Last(); }
	FORCEINLINE int8 GetPriority() const { return Priority; }
	bool Dequeue(TSubclassOf<UPrimitiveTask>& OutElem);
	bool HasPlan() const { return Plan.Num() > 0; }

	FTaskResult LastResult;

	FText GetName() const {return PlanName;}
	
private:
	TDeque<TSubclassOf<UPrimitiveTask>> Plan;
	int8 Priority = MAX_int8;
	FText PlanName;
	
	friend class FPlanner;
};

// Simple object that handles making HTN plans.
class FPlanner
{
	
public:
	enum EPlanResult: uint8
	{
		NoTasks,
		Success,
		Failed
	};

	
	explicit FPlanner(const TArray<TSoftObjectPtr<UTask>>& InTasks, const FWorldStateContainer* InWorldStates);

	// Requests a new plan. OutPlan is only modified if the new plan has higher priority
	// OutPlan - Plug current-use plan
	bool RequestPlan(FHTNPlan& OutPlan, bool bLogDebug = false) const;
	
private:
	// makes a new plan
	EPlanResult MakePlan(FHTNPlan& OutPlan) const;
	
	const TArray<TSoftObjectPtr<UTask>>& Tasks;
	const FWorldStateContainer* WorldStates;
	FWorldStateContainer TasksStates;
};