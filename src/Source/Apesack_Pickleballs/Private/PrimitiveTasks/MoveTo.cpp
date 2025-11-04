// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/MoveTo.h"



void UMoveTo::Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback)
{
	Super::Initialize(InstigatorActor, OnCompleteCallback);

	const FTaskResult PreviousResult = GetPreviousTaskResult();
	TargetActor = PreviousResult.TargetActor;
	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
	else
	{
		TargetLocation = PreviousResult.PointOfInterest;

		DirToTarget = TargetLocation - Instigator->GetActorLocation();
		const float DotProd = FVector::DotProduct(DirToTarget, Instigator->GetActorForwardVector());
		if (DotProd < 0)
		{
			// target is behind us
			Instigator->AddActorLocalRotation(FRotator(0.f, 180.f, 0.f));
		}
	}

	InstigatorAsPawn = Cast<APawn>(InstigatorActor);
	
	StopDistSquared = FMath::Square(StopDist);
	GotoDirectDistanceSquared = FMath::Square(GotoDirectDistance);
	OutOfSightDistanceSquared = FMath::Square(OutOfSightDistance);
}

void UMoveTo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// update target location
	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();

		// get direction to target
		DirToTarget = TargetLocation - Instigator->GetActorLocation();

		// rotate character
		const float DotProd = FVector::DotProduct(DirToTarget, Instigator->GetActorForwardVector());
		if (DotProd < 0)
		{
			// target is behind us
			Instigator->AddActorLocalRotation(FRotator(0.f, 180.f, 0.f));
		}
	}

	const float TargetDistanceSquared = FVector::DistSquared2D(Instigator->GetActorLocation(), TargetLocation);
	
	// move actor if conditions permit
	if (TargetDistanceSquared > OutOfSightDistanceSquared)
	{
		OnTaskCompleted(FTaskResult(ETaskState::Failed, EffectContainer, TargetActor, FString("Target went out of sight!")));
		return;
	}
	
	if (TargetDistanceSquared <= GotoDirectDistanceSquared)
	{
		// veer off character's set radius to directly to the target
		InstigatorAsPawn->AddMovementInput(DirToTarget.GetUnsafeNormal2D());
	}
	else
	{
		// walk forward
		InstigatorAsPawn->AddMovementInput(InstigatorAsPawn->GetActorForwardVector());
	}

	if (TargetDistanceSquared <= StopDistSquared)
	{
		OnTaskCompleted(FTaskResult(ETaskState::Success, EffectContainer, TargetActor, FString()));
	}
}
