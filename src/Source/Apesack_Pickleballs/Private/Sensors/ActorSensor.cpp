// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensors/ActorSensor.h"
#include "Buildings/BuildingBase.h"
#include "HTN/HTNComponent.h"
#include "Items/ItemActor.h"
#include "Kismet/GameplayStatics.h"
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
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Owner->GetOwner());
		Params.bTraceComplex = false;                
		Params.bReturnPhysicalMaterial = false;
		const FVector StartLocation = Owner->GetOwner()->GetActorLocation() + FVector::UpVector * 40.f;
		
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

			if (SenseSuccess)
			{
				WorldState.Value = true;
				ReceiveOnSensed();
			}
		}
	}
	Owner->UpdateWorldState(WorldState);
}