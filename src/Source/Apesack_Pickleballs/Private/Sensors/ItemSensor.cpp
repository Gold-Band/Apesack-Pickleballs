// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensors/ItemSensor.h"
#include "ItemActor.h"
#include "Kismet/KismetSystemLibrary.h"

void UItemSensor::Tick()
{
	Super::Tick();

	WorldState.Value = false;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> OutActors;

	if (UKismetSystemLibrary::SphereOverlapActors(Owner->GetWorld(), Owner->GetActorLocation(), SenseRadius,ObjectTypes, AItemActor::StaticClass(), TArray<AActor*>(),OutActors))
	{
		for (const auto& Actor : OutActors)
		{
			if (Cast<AItemActor>(Actor)->HasMatchingGameplayTag(ObjectTag))
			{
				WorldState.Value = true;
				ReceiveOnSensed();
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("UItemSensor::Tick"));
	if (OnSensed) OnSensed(WorldState);
	else UE_LOG(LogTemp, Warning, TEXT("UItemSensor::Tick - OnSensed not bound!"));
}

void UItemSensor::Initialize(AActor* OwnerActor, const FOnSenseCallback& OnSenseCallback)
{
	Super::Initialize(OwnerActor, OnSenseCallback);
}
