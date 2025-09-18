// Fill out your copyright notice in the Description page of Project Settings.


#include "SunClockSynchronizer.h"

// Sets default values for this component's properties
USunClockSynchronizer::USunClockSynchronizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}


// Called when the game starts
void USunClockSynchronizer::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USunClockSynchronizer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USunClockSynchronizer::InitializeComponent()
{
	Super::InitializeComponent();
	UWorldClockSubsystem::Get(this)->OnTimeTickedDelegate.AddUniqueDynamic(this, &ThisClass::SyncRotationToTime);
}

static int GetTimeDifferenceSeconds(const FTimestamp& FromTime, const FTimestamp& ToTime)
{
	const int FromSeconds = FromTime.Second + FromTime.Minute*60 + FromTime.Hour*3600 + FromTime.Day*86400;
	const int ToSeconds = ToTime.Second + ToTime.Minute*60 + ToTime.Hour*3600 + ToTime.Day*86400;
	return ToSeconds - FromSeconds;
}

void USunClockSynchronizer::SyncRotationToTime(const FTimestamp& Time)
{
	if (!bSyncToWorldClock) return;
	
	const int DeltaTime = GetTimeDifferenceSeconds(PreviousTime, Time);
	TotalSecondsToday = (TotalSecondsToday + DeltaTime) % 86400;
	AActor* Owner = GetOwner();
	if (Owner && !RotationCurve.IsNull())
	{
		float SetAngle = RotationCurve.LoadSynchronous()->GetFloatValue(TotalSecondsToday/86400.0f) * 360.0f - 90.f;
		
		// Normalize axis
		const FVector Axis = RotationAxis.GetSafeNormal();

		// Build a rotation quaternion
		const FQuat NewQuat = FQuat(Axis, FMath::DegreesToRadians(SetAngle));

		// Apply to actor
		Owner->SetActorRotation(NewQuat);
	}

	PreviousTime = Time;
}

