// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/CheckRadius.h"
#include "Movement/CircularPawnMovementComponent.h"

void UCheckRadius::Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback)
{
	Super::Initialize(InstigatorActor, OnCompleteCallback);
}

void UCheckRadius::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const auto MovementComp = Cast<UCircularPawnMovementComponent>(Instigator->GetComponentByClass(UCircularPawnMovementComponent::StaticClass()));
	if (!MovementComp)
	{
		OnTaskCompleted(FTaskResult(ETaskState::Failed, !EffectContainer, nullptr, FString("Failed! No movement component!")));
		return;
	}
	
	const float DistDiff = FMath::Abs(MovementComp->GetDefaultRadius() - Instigator->GetActorLocation().Size2D());
	
	if (DistDiff <= AcceptableMargin)
	{
		EffectContainer.Add(FWorldState(FName("IsOnRadius"), true));
	}
	else
	{
		EffectContainer.Add(FWorldState(FName("IsOnRadius"), false));
	}

	OnTaskCompleted(FTaskResult(ETaskState::Success, EffectContainer, nullptr, FString()));
}
