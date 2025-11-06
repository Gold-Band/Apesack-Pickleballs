// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Planner.h"
#include "Sensor.h"
#include "HTNComponent.generated.h"


// The HTN Domain for the parent actor.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UHTNComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHTNComponent();

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void SetTasks(const TArray<TSoftObjectPtr<UTask>>& NewTasks);

	UFUNCTION(BlueprintPure, Category = "HTN")
	const FTaskResult& GetPreviousTaskResult() const {return Plan.LastResult;}
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CancelActivePlan();
	void RunTask(const TSoftObjectPtr<UTask> Task);

	UFUNCTION(BlueprintCallable, Category = "HTN")
	void UpdateWorldState(const FString& OverrideStateName, bool OverrideValue);
	void UpdateWorldState(const FWorldState& UpdatedWorldState);

	bool VerifyWorldState(const FWorldStateContainer& VerifyContainer) const;
	
private:
	virtual void InitializeComponent() override;
	TObjectPtr<UPrimitiveTask> GetNextTaskInitialized(FHTNPlan& InPlan);
	
	//** Domain Stuff **//
	FWorldStateContainer WorldStateContainer;

	// the list of world states that we care about
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"), Category="HTN")
	TSoftObjectPtr<UDataTable> Domain;
	
	//** Sensor Stuff **//
	UPROPERTY()
	TArray<TObjectPtr<USensor>> SensorInstances;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"), Category="HTN")
	TArray<FSensorInitializer> Sensors;
	
	//** Task Stuff **//
	TArray<TSoftObjectPtr<UTask>> Tasks;
	UPROPERTY()
	TObjectPtr<UPrimitiveTask> CurrentTask;

	bool bGetNextTask = false;

	//** Plan Stuff **//
	FHTNPlan Plan;
	TUniquePtr<FPlanner> Planner;
	
	UPROPERTY(EditAnywhere, Category = "HTN")
	float PlanningInterval = 1.0f;
	float LastPlan;

	//** Other **//
	TFunction<void()> PreTickEvent;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"), Category = "HTN")
	bool bLogDebug = false;

	// For seeing world states in game - purely "debug" purposes
	friend class UWorldStateWidgetComponent;
};