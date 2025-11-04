#include "HTN/Sensor.h"

#include "HTN/HTNComponent.h"

void USensor::Initialize(UHTNComponent* OwnerDomain)
{
	WorldState = WorldStateMaker.ToWorldState();
	Owner = OwnerDomain;
}

void USensor::Tick()
{
	//UE_LOG(LogTemp, Warning, TEXT("Sensor::Tick() - %s"), *WorldState.Name.ToString())
	if (TickInterval>0 && GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(Owner,EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			LastTick = World->GetTimeSeconds();
		}
	}
	ReceiveTick();
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