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
	FTaskResult Result{ETaskState::Failed, FWorldStateContainer(), nullptr, FString()};
	
	if (InstigatorAsNpc->HasGuardingWall())
	{
		FVector GotoLocation;
		if (Zone == 1) GotoLocation = UKismetMathLibrary::RandomPointInBoundingBox_Box(InstigatorAsNpc->GetGuardingWall()->GetMeleeDefendBox());
		else if (Zone == 2) GotoLocation = UKismetMathLibrary::RandomPointInBoundingBox_Box(InstigatorAsNpc->GetGuardingWall()->GetArcherDefendBox());
	
		GotoLocation = GotoLocation.GetSafeNormal2D() * InstigatorAsNpc->GetCharacterPreferredRadius();
		if (bPrintStatusInLog) UE_LOG(LogTemp, Warning, TEXT("Defense position = %s"), *GotoLocation.ToString())
		Result.PointOfInterest = GotoLocation;
		Result.EndState = ETaskState::Success;
		OnTaskCompleted(Result);
		return;
	}
	
	OnTaskCompleted(Result);
}
