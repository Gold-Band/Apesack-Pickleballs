// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensors/ActorSensor.h"

#include "HTN/HTNComponent.h"
#include "Items/ItemActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NPC/NpcBase.h"

void UActorSensor::Tick()
{
	Super::Tick();

	WorldState.Value = false;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TArray<AActor*> OutActors;

	if (!FilterClass) FilterClass = AActor::StaticClass();

	// check if any (filterClass) type actors are in the radius
	if (UKismetSystemLibrary::SphereOverlapActors(Owner->GetWorld(), Owner->GetOwner()->GetActorLocation(), SenseRadius,ObjectTypes, FilterClass, TArray<AActor*>(),OutActors))
	{
		for (const auto& Actor : OutActors)
		{
			// for now.. we can only really sense items and npcs
			if (FilterClass->IsChildOf(AItemActor::StaticClass()))
			{
				if (Cast<AItemActor>(Actor)->HasMatchingGameplayTag(ObjectTag))
				{
					WorldState.Value = true;
					ReceiveOnSensed();
				}
			}
			else if (FilterClass->IsChildOf(ANpcBase::StaticClass()))
			{
				if (Cast<ANpcBase>(Actor)->HasMatchingGameplayTag(ObjectTag))
				{
					WorldState.Value = true;
					ReceiveOnSensed();
				}
			}
		}
	}
	Owner->UpdateWorldState(WorldState);
}