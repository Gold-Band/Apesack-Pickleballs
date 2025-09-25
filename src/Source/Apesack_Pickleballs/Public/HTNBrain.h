// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoapSubsystem.h"
#include "Components/ActorComponent.h"
#include "HTNBrain.generated.h"


class UPrimitiveTask;
class UFloatingPawnMovement;
class USensor;
class UTask;
class UWorldStateVariable;
class ANpc;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APESACK_PICKLEBALLS_API UHTNBrain : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHTNBrain();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "HTN Brain")
	void OnWorldStateChanged(const FWorldState& NewState);
	
	UFUNCTION(BlueprintCallable, Category = "HTN Brain")
	void OnWorldStateVariableChanged(const FWorldStateVariable& NewVariable);

private:
	bool bIsPlanInProgress = false;
	bool bRunPlan= false;
	bool bPlanCompleted= true;
		
	UPROPERTY(EditAnywhere)
	FWorldState WorldState;
	
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UTask>> MainTasks;

	TQueue<TObjectPtr<UPrimitiveTask>> Plan;

	UPROPERTY()
	TObjectPtr<UPrimitiveTask> CurrentTask;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;

	enum EPlanResult : uint8
	{
		Success,
		InProgress,
		NoTasks,
		Failed
	};
	
	EPlanResult MakePlan();
	void NextTask();
	void NewPlan();
	
	friend class UGoapDisplayer;
};