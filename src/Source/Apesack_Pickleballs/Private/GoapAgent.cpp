// Fill out your copyright notice in the Description page of Project Settings.
#include "GoapAgent.h"
#include "Npc.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UGoapAgent::UGoapAgent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = 0.25f;
	// ...

	Npc = Cast<ANpc>(GetOwner());	
	if (!Npc) return;

	// attack example
	FAction Action1;
	Action1.Cost = 3;
	Action1.Effect.Variables.Add(EWorldStateOption::HasTarget,false);
	Action1.Requirement.Variables.Add(EWorldStateOption::HasTarget, true);
	Action1.Requirement.Variables.Add(EWorldStateOption::CanAttack, true);
	Actions.Add(Action1);

	// kill target example
	FWorldState Goal1;
	Goal1.Variables.Add(EWorldStateOption::HasTarget, false);
	Goals.Add(Goal1);
	FWorldState Goal2;
	Goal2.Name = "BeEquipped";
	Goal2.Variables.Add(EWorldStateOption::IsEquipped, true);
	Goals.Add(Goal2);
	

	// my world states example
	CurrentState.Variables.Add(EWorldStateOption::IsEquipped, false);
	CurrentState.Variables.Add(EWorldStateOption::HasTarget, false);
	CurrentState.Variables.Add(EWorldStateOption::CanAttack, false);
	CurrentState.Variables.Add(EWorldStateOption::CanMove, true);
}


// Called when the game starts
void UGoapAgent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("UGoapAgent::BeginPlay"));
	// ...
	const TFunction<void(const FPlanResult&)> OnPlanComplete = [&](const FPlanResult& Result)
	{
		bIsPlanInProgress = false;
		UE_LOG(LogTemp, Warning, TEXT("OnPlanComplete | Result:Sucess=%s | Message:%s"), Result.bSuccess? TEXT("True") : TEXT("False"), *Result.Message);
	};

	if (!bIsPlanInProgress)
	{
		bIsPlanInProgress = true;
		UGoapSubsystem::RequestPlan(CurrentState, Goals[0], Actions, OnPlanComplete);
	}


}


// Called every frame
void UGoapAgent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsEquipped && !bIsPlanInProgress)
	{
		// Scan for nearest tool
		
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tool"),FoundActors);

		// save closest tool direction
		float DistToNearest = FLT_MAX;
		for (const AActor* Actor : FoundActors)
		{
			const float Dist = GetOwner()->GetActorLocation().X - FoundActors[0]->GetActorLocation().X;
			if (FMath::Abs(Dist) < FMath::Abs(DistToNearest)) DistToNearest = Dist;
		}
	}
}

void UGoapAgent::Move(EDirection2D Direction)
{
	
}
