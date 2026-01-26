// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/ArcherTower.h"

#include "AI/NPC/Npc.h"

AArcherTower::AArcherTower()
{
	PrimaryActorTick.bCanEverTick = false;
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	TowerMesh->SetupAttachment(RootComponent);
	
	Name = FString("Archer Tower");
}

bool AArcherTower::HasRoom() const
{
	return NumOccupants < MaxOccupants;
}

void AArcherTower::AddOccupant(ANpc* NewOccupant)
{
	if (!HasRoom()) return;

	Occupants.Add(NewOccupant);
	NewOccupant->SetActorLocation(Spots[NumOccupants]->GetComponentLocation());
	
	NumOccupants++;
}

void AArcherTower::RemoveOccupant(ANpc* OldOccupant)
{
	Occupants.Remove(OldOccupant);
	NumOccupants--;
}

void AArcherTower::AddSpot(USceneComponent* NewSpot)
{
	Spots.Add(NewSpot);
}
 
TArray<UListItemObject*> AArcherTower::GetInfo() const
{
	// hp
	// num occupans / max occupants
	return Super::GetInfo();
}

TArray<UListItemObject*> AArcherTower::GetActions() const
{
	// upgrades
	return Super::GetActions();
}

void AArcherTower::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Building ArcherTower"));
	UBuildingsManager::Get(GetWorld())->AddBuilding(this, EBuildingType::Tower);
}
