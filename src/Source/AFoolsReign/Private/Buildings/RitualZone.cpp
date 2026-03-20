// Fill out your copyright notice in the Description page of Project Settings.
#include "Buildings/RitualZone.h"
#include "Components/BoxComponent.h"
#include "WorldClock/WorldClockSubsystem.h"

ARitualZone::ARitualZone()
{
	PrimaryActorTick.bCanEverTick = false;
	OverlapArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	OverlapArea->SetupAttachment(RootComponent);
	
	Name = FString("Ritual Grounds");
	BuildingType = EBuildingType::Ritual;
}

void ARitualZone::AddOccupant(AActor* NewOccupant)
{
	// set actor location to a spot
	if (!HasRoom()) return;
	
	OccupantCount++;
}

void ARitualZone::RemoveOccupant()
{
	OccupantCount--;
}

bool ARitualZone::HasRoom() const
{
	return OccupantCount < MaxOccupants;
}

void ARitualZone::BeginPlay()
{
	Super::BeginPlay();
	
	UWorldClockSubsystem::Get(GetWorld())->OnDayTickedDelegate.AddUniqueDynamic(this, &ThisClass::OnDayTicked);
}

void ARitualZone::OnDayTicked(uint8 Day)
{
	MaxOccupants = FMath::Min(MaxOccupants + Day%2, MaxSpots); // cap it a 10
	//UE_LOG(LogTemp, Warning, TEXT("Max occupants = %i"), MaxOccupants);
}
