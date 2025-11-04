// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTN/Sensor.h"
#include "ActorSensor.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UActorSensor : public USensor
{
	GENERATED_BODY()

public:
	virtual void Tick() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnSensed"))
	void ReceiveOnSensed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SenseRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ObjectTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* FilterClass;
};