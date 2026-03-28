// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldClock/SunClockSynchronizer.h"

// Sets default values for this component's properties
USunClockSynchronizer::USunClockSynchronizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}

void USunClockSynchronizer::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SyncRotationToTime();
}


void USunClockSynchronizer::InitializeComponent()
{
	Super::InitializeComponent();
	
	WorldClock = UWorldClockSubsystem::Get(this);
	//WorldClock->OnTimeTickedDelegate.AddUniqueDynamic(this, &ThisClass::SyncRotationToTime);
}


static int GetTimeDifferenceSeconds(const FTimestamp& FromTime, const FTimestamp& ToTime)
{
	const int FromSeconds = FromTime.Second + FromTime.Minute*60 + FromTime.Hour*3600 + FromTime.Day*86400;
	const int ToSeconds = ToTime.Second + ToTime.Minute*60 + ToTime.Hour*3600 + ToTime.Day*86400;
	return ToSeconds - FromSeconds;
}
void USunClockSynchronizer::SyncRotationToTime()
{
	if (!bSyncToWorldClock)
		return;

	AActor* Owner = GetOwner();
	if (!Owner || RotationCurve.IsNull())
		return;

	const FTimestamp Time = WorldClock->GetTime();
	// --- Compute absolute normalized time (no accumulation drift) ---
	const int DeltaTime = GetTimeDifferenceSeconds(PreviousTime, Time);

	TotalSecondsToday += DeltaTime;

	// Prevent wrap snapping
	if (TotalSecondsToday >= 86400)
		TotalSecondsToday -= 86400;
	else if (TotalSecondsToday < 0)
		TotalSecondsToday += 86400;

	//const float NormalizedTime = static_cast<float>(TotalSecondsToday) / 86400.0f;
	
	
	// * * //
	const double TimeSince = GetWorld()->TimeSince(LastTime);
	LastTime = TimeSince >= 86400? TimeSince : LastTime;
	const float NormalizedTime = GetWorld()->TimeSince(LastTime) / 86400;

	const float SetAngle = RotationCurve.LoadSynchronous()->GetFloatValue(NormalizedTime) * 360.0f - 90.f;

	
	const FVector Axis = RotationAxis.GetSafeNormal();
	const FQuat TargetQuat(Axis, FMath::DegreesToRadians(SetAngle));

	
	const FQuat CurrentQuat = Owner->GetActorQuat();

	const float InterpSpeed = 2.0f; 
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	const FQuat SmoothedQuat =
		FQuat::Slerp(CurrentQuat, TargetQuat, DeltaSeconds * InterpSpeed).GetNormalized();

	Owner->SetActorRotation(SmoothedQuat);

	PreviousTime = Time;
}

