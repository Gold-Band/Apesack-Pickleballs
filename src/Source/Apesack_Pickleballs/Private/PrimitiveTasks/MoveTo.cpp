// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/MoveTo.h"

#include "HTN/HTNComponent.h"
#include "NPC/NpcBase.h"


void UMoveTo::Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback)
{
	Super::Initialize(InstigatorActor, OnCompleteCallback);

	Npc = Cast<ANpcBase>(InstigatorActor);
	
	const FTaskResult PreviousResult = GetPreviousTaskResult();
	TargetActor = PreviousResult.TargetActor;
	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
		CheckOrientationToTarget();
	}
	else
	{
		TargetLocation = PreviousResult.PointOfInterest;
		CheckOrientationToTarget();
	}
	
	StopDistSquared = FMath::Square(StopDist);
	GotoDirectDistanceSquared = FMath::Square(GotoDirectDistance);
	OutOfSightDistanceSquared = FMath::Square(OutOfSightDistance);

	RequirementContainer = FWorldStateContainer::FromArray(Requirement);
	
	HTNDomain = Cast<UHTNComponent>(InstigatorActor->GetComponentByClass(UHTNComponent::StaticClass()));
}

void UMoveTo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// check if we are still good to go
	if (HTNDomain && !HTNDomain->VerifyWorldState(RequirementContainer))
	{
		// failed
		if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("Moveto verify fail"))
		OnTaskCompleted(FTaskResult(ETaskState::Failed, !EffectContainer, TargetActor, FString("Target is unreachable!")));
	}
	
	// update target location
	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}

	const float TargetDistanceSquared = FVector::DistSquared2D(Instigator->GetActorLocation(), TargetLocation);
	
	// move actor if conditions permit
	if (TargetDistanceSquared > OutOfSightDistanceSquared)
	{
		if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("Moveto Target went out of sight!"))
		OnTaskCompleted(FTaskResult(ETaskState::Failed, !EffectContainer, TargetActor, FString("Target went out of sight!")));
		return;
	}
	
	if (TargetDistanceSquared <= StopDistSquared)
	{
		if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("Moveto Success"))
		OnTaskCompleted(FTaskResult(ETaskState::Success, EffectContainer, TargetActor, FString("Success")));
		return;
	}

	if (TargetDistanceSquared <= GotoDirectDistanceSquared)
	{
		// veer off character's set radius to directly to the target
		Npc->AddMovementInput(DirToTarget.GetUnsafeNormal2D());
	}
	else
	{
		// walk forward
		Npc->AddMovementInput(Npc->GetActorForwardVector(), Direction);
	}
}

void UMoveTo::CheckOrientationToTarget()
{
	// get direction to target
	DirToTarget = TargetLocation - Instigator->GetActorLocation();
	// rotate character
	const float DotProd = FVector::DotProduct(DirToTarget, Npc->GetForwardVector());
	if (DotProd < 0)
	{
		// target is behind us
		if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("Moveto flip"))
		//Npc->Flip();
		if (Direction == 1) Direction = -1;
		else Direction = 1;
	}
}
