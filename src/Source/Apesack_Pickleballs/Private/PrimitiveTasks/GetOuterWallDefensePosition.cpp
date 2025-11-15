// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTasks/GetOuterWallDefensePosition.h"

#include "Buildings/Wall.h"
#include "Kismet/KismetMathLibrary.h"
#include "NPC/NpcFriendly.h"

void UGetOuterWallDefensePosition::Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback)
{
	Super::Initialize(InstigatorActor, OnCompleteCallback);
	
	InstigatorAsNpc = Cast<ANpcFriendly>(InstigatorActor);
}

void UGetOuterWallDefensePosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector GotoLocation = UKismetMathLibrary::RandomPointInBoundingBox_Box(InstigatorAsNpc->GetGuardingWall()->GetArcherDefendBox());
	GotoLocation = GotoLocation.GetSafeNormal2D() * InstigatorAsNpc->GetCharacterPreferredRadius();
	FTaskResult Result{ETaskState::Success, FWorldStateContainer(), nullptr, FString()};
	Result.PointOfInterest = GotoLocation;
	
	OnTaskCompleted(Result);
}
