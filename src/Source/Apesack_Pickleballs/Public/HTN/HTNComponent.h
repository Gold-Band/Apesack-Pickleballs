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

private:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	TObjectPtr<UPrimitiveTask> GetNextTaskInitialized(FHTNPlan& InPlan);
	
	//** Sensor Stuff **//
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	TArray<FSensorInitializer> Sensors;
	TArray<TObjectPtr<USensor>> SensorInstances;
	FWorldStateContainer WorldStateContainer;

	
	//** Task Stuff **//
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	TArray<TSoftObjectPtr<UTask>> Tasks;
	
	TObjectPtr<UPrimitiveTask> CurrentTask;

	bool bGetNextTask = false;


	//** Plan Stuff **//
	FHTNPlan Plan;
	TUniquePtr<FPlanner> Planner;
	
	UPROPERTY(EditAnywhere, Category = "Brain")
	float PlanningInterval = 1.0f;
	float LastPlan;

	//** Other **//
	TFunction<void()> PreTickEvent;

	// For seeing world states in game - purely "debug" purposes
	friend class UWorldStateWidgetComponent;
};