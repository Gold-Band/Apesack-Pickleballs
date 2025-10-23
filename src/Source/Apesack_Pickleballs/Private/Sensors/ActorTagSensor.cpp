#include "Sensors/ActorTagSensor.h"
#include "Kismet/KismetSystemLibrary.h"

void UActorTagSensor::Tick()
{
	Super::Tick();
	
	WorldState.Value = false;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> OutActors;

	if (UKismetSystemLibrary::SphereOverlapActors(Owner->GetWorld(), Owner->GetActorLocation(), SenseRadius,ObjectTypes, AActor::StaticClass(), TArray<AActor*>(),OutActors))
	{
		for (const auto& Actor : OutActors)
		{
			if (Actor->ActorHasTag(SearchTag))
			{
				WorldState.Value = true;
				ReceiveOnSensed();
			}
		}
	}
	if (OnSensed) OnSensed(WorldState);
	else UE_LOG(LogTemp, Warning, TEXT("UActorTagSensor::Tick - OnSensed not bound!"));
}

