// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HTNComponent.generated.h"


class FTask;

// The HTN Domain for the parent actor.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UHTNComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHTNComponent();

	void AssignTask(FTask* Task, int Priority = -1);

	void UpdatePlan();
	
	FTask* CurrentTask = nullptr;
	bool bHasValidTask = false;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	TArray<FTask*> Tasks;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"), Category = "HTN")
	bool bPrintDebug = false;
	
	// For seeing world states in game - purely "debug" purposes
	friend class UWorldStateWidgetComponent;
};