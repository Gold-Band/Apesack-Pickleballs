#include "AI/HTN/Sensor.h"
#include "AI/HTN/HTNComponent.h"
#include "Managers/SensorManager.h"

void USensor::Initialize(UHTNComponent* OwnerDomain)
{
	WorldState = WorldStateMaker.ToWorldState();
	Owner = OwnerDomain;
	WorldContext = Owner->GetWorld();
	SensorManager = USensorManager::Get(WorldContext);
}

void USensor::Tick()
{
	if (TickInterval>0)
	{
		WaitTime = 0;
	}
	ReceiveTick();
}

bool USensor::ShouldTick(float DeltaTime)
{
	if (TickInterval==0) return true;

	return (WaitTime+=DeltaTime) >= TickInterval;
}