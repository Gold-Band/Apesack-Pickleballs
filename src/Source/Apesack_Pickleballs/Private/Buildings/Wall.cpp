// Fill out your copyright notice in the Description page of Project Settings.
#include "Buildings/Wall.h"

#include "Managers/BuildingsManager.h"

AWall::AWall()
{
	PrimaryActorTick.bCanEverTick = false;
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(RootComponent);
	
}

void AWall::BeginPlay()
{
	Super::BeginPlay();
	
	UBuildingsManager::Get(GetWorld())->AddBuilding(this, EBuildingType::Wall);
}
