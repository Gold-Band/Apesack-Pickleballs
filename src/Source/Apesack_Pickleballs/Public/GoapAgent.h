// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoapSubsystem.h"
#include "Components/ActorComponent.h"
#include "GoapAgent.generated.h"


class ANpc;

enum class EDirection2D : int8
{
	Stop = 0,
	Left = -1,
	Right = 1
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APESACK_PICKLEBALLS_API UGoapAgent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoapAgent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Move(EDirection2D Direction);
	
private:
	bool bIsPlanInProgress = false;
	bool bIsEquipped = false;
	
	
	
	UPROPERTY(EditAnywhere)
	TArray<FAction> Actions;

	UPROPERTY(EditAnywhere)
	TArray<FWorldState> Goals;

	UPROPERTY(EditAnywhere)
	FWorldState CurrentState;
	
	UPROPERTY()
	ANpc* Npc;

	TQueue<TFunction<void()>> Plan;

	friend class UGoapDisplayer;
};
