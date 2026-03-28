// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldClock/SunClockSynchronizer.h"

// Sets default values for this component's properties
USunClockSynchronizer::USunClockSynchronizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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


void USunClockSynchronizer::SyncRotationToTime()
{
	if (!bSyncToWorldClock)
		return;

	AActor* Owner = GetOwner();
	if (!Owner || RotationCurve.IsNull())
		return;
	

	const float NormalizedTime = WorldClock->GetNormalizedTime();
	
	
	//UE_LOG(LogTemp, Warning, TEXT("Normal=%f"), NormalizedTime)
	const float SetAngle = RotationCurve.LoadSynchronous()->GetFloatValue(NormalizedTime) * 360.0f - 90.f;

	
	const FVector Axis = RotationAxis.GetSafeNormal();
	const FQuat TargetQuat(Axis, FMath::DegreesToRadians(SetAngle));

	
	const FQuat CurrentQuat = Owner->GetActorQuat();

	const float InterpSpeed = 2.0f; 
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	const FQuat SmoothedQuat =
		FQuat::Slerp(CurrentQuat, TargetQuat, DeltaSeconds * InterpSpeed).GetNormalized();

	Owner->SetActorRotation(SmoothedQuat);

	//PreviousTime = Time;
}

