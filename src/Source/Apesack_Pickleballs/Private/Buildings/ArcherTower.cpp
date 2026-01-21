// Fill out your copyright notice in the Description page of Project Settings.

#include "Buildings/ArcherTower.h"

AArcherTower::AArcherTower()
{
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(RootComponent);
}


void AArcherTower::BeginPlay()
{
	Super::BeginPlay();
	
}
