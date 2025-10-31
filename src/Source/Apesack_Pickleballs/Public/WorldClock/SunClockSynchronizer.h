// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "WorldClockSubsystem.h"
#include "SunClockSynchronizer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APESACK_PICKLEBALLS_API USunClockSynchronizer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USunClockSynchronizer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	
private:
	float CurrentAngle;
	
	UPROPERTY(EditAnywhere)
	FVector RotationAxis = FVector(0, 1, 0);
	
	int TotalSecondsToday = 0;

	FTimestamp PreviousTime;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UCurveFloat> RotationCurve;

	UPROPERTY(EditAnywhere)
	bool bSyncToWorldClock = true;
	
private:
	UFUNCTION()
	void SyncRotationToTime(const FTimestamp& Time);
};
