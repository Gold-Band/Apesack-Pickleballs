#include "HTN/Sensor.h"

void USensor::Initialize(AActor* OwnerActor, const FOnSenseCallback& OnSenseCallback)
{
	WorldState = WorldStateMaker.ToWorldState();
	WorldState = WorldStateMaker.ToWorldState();
	OnSensed = OnSenseCallback;
	Owner = OwnerActor;
	//ReceiveInitialize(OwnerActor);
}

void USensor::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Sensor::Tick() - %s"), *WorldState.Name.ToString())
	if (TickInterval>0) LastTick = GetWorld()->GetTimeSeconds();
	ReceiveTick(DeltaTime);
}

bool USensor::ShouldTick() const
{
	if (TickInterval==0) return true;

	// if no world, no GetTimeSeconds()
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(Owner,EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			return false;
		}
	
		return World->GetTimeSeconds() - LastTick >= TickInterval;
	}
	return false;
}