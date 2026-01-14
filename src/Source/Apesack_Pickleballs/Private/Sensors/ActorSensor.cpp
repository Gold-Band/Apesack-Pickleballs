// Fill out your copyright notice in the Description page of Project Settings.
#include "Sensors/ActorSensor.h"
#include "HTN/HTNComponent.h"
#include "Managers/SensorManager.h"


void UActorSensor::Tick()
{
	Super::Tick();

	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ItemSensor")

	WorldState.Value = false;

	const auto Result = SensorManager->FindNearestWithTag(ObjectTag, Owner->GetOwner()->GetActorLocation());
	if (Result)
	{
		WorldState.Value = true;
		ReceiveOnSensed();
	}
	
	Owner->UpdateWorldState(WorldState);

	/*if (!FilterClass) FilterClass = AActor::StaticClass();
		for (const auto& Actor : OutActors)
		{

			if (bLineOfSightRequired)
			{
				const FVector EndLocation = Actor->GetActorLocation() + FVector::UpVector * 40.f;
				// check for line of sight
				const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation, EndLocation, ECC_Visibility, Params);
				if (bHit && HitResult.GetActor() != Actor)
				{
					//UE_LOG(LogTemp, Warning, TEXT("HitActor = %s"), *HitResult.GetActor()->GetActorNameOrLabel())	
					continue;
				}
			}

			bool SenseSuccess = false;
			
			// for now.. we can only really sense items, npcs and walls
			if (FilterClass->IsChildOf(AItemActor::StaticClass()))
			{
				if (Cast<AItemActor>(Actor)->HasMatchingGameplayTag(ObjectTag))
				{
					SenseSuccess = true;
				}
			}
			else if (FilterClass->IsChildOf(ANpcBase::StaticClass()))
			{
				if (Cast<ANpcBase>(Actor)->HasMatchingGameplayTag(ObjectTag))
				{
					SenseSuccess = true;
				}
			}
			else if (FilterClass->IsChildOf(ABuildingBase::StaticClass()))
			{
				if (Cast<ABuildingBase>(Actor)->HasMatchingGameplayTag(ObjectTag))
				{
					SenseSuccess = true;
				}
			}
		}
	}*/
}
