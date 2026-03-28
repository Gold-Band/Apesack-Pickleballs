// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "WorldClockSubsystem.h"
#include "SunClockSynchronizer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AFOOLSREIGN_API USunClockSynchronizer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USunClockSynchronizer();

protected:
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
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
	
	UPROPERTY()
	UWorldClockSubsystem* WorldClock;
	
	double LastTime = 0;
	
private:
	UFUNCTION()
	void SyncRotationToTime();
};
