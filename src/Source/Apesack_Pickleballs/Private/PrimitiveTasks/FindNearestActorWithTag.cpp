// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/FindNearestActorWithTag.h"

#include "Kismet/KismetSystemLibrary.h"

void UFindNearestActorWithTag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(Instigator->GetWorld(), Instigator->GetActorLocation(), Radius,ObjectTypes, AActor::StaticClass(), TArray<AActor*>(),OutActors);
	
	FTaskResult Objects{ETaskState::Failed, !EffectContainer, nullptr, FString("FindNearestActorWithTag Failed!")};

	float Closest = FLT_MAX;
	for (const auto& Actor : OutActors)
	{
		if (Actor->ActorHasTag(SearchTag))
		{
			const float Dist = FVector::DistSquaredXY(Instigator->GetActorLocation(), Actor->GetActorLocation());
			if (Dist< Closest)
			{
				Objects.TargetActor = Actor;
				Closest = Dist;
			}
			Objects.EndState = ETaskState::Success;
			Objects.Effect = EffectContainer;
			Objects.Message = FString("FindNearestActorWithTag Succeeded!");
		}
	}
	
	OnTaskCompleted(Objects);
}
