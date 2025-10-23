// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTN/Sensor.h"
#include "ItemSensor.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UItemSensor : public USensor
{
	GENERATED_BODY()

public:
	virtual void Tick() override;
	virtual void Initialize(AActor* OwnerActor, const FOnSenseCallback& OnSenseCallback) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnSensed"))
	void ReceiveOnSensed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SenseRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ObjectTag;
};