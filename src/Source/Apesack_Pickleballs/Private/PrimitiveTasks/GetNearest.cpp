// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/GetNearest.h"

#include "GameplayTagAssetInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UGetNearest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//pulse
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(Instigator->GetWorld(), Instigator->GetActorLocation(), 1000.f,ObjectTypes, AActor::StaticClass(), TArray<AActor*>(),OutActors);
	
	FTaskResult Objects{ETaskState::Failed, !EffectContainer, nullptr, FString("GetNearest Failed!")};

	float closest = FLT_MAX;
	for (const auto& Actor : OutActors)
	{
		IGameplayTagAssetInterface* Interface = Cast<IGameplayTagAssetInterface>(Actor);
		if (Interface && Interface->HasMatchingGameplayTag(ObjectTag))
		{
			const float Dist = FVector::DistSquaredXY(Instigator->GetActorLocation(), Actor->GetActorLocation());
			if ( Dist< closest)
			{
				Objects.TargetActor = Actor;
				closest = Dist;
			}
			Objects.EndState = ETaskState::Success;
			Objects.Effect = EffectContainer;
			Objects.Message = FString("GetNearest Succeeded!");
		}
	}
	
	OnTaskCompleted(Objects);
}
