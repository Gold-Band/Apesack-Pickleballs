// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/Wall.h"

#include "Buildings/Zone.h"
#include "GameModes/DefaultGameMode.h"

AWall::AWall()
{
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(RootComponent);
	
	MeleeDefendZone = CreateDefaultSubobject<UZone>(TEXT("MeleeZone"));
	MeleeDefendZone->SetupAttachment(RootComponent);
	MeleeDefendZone->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	MeleeDefendZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	MeleeDefendZone->SetMobility(EComponentMobility::Type::Static);
	MeleeDefendZone->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	MeleeDefendZone->SetGenerateOverlapEvents(true);
	
	ArcherDefendZone = CreateDefaultSubobject<UZone>(TEXT("ArcherZone"));
	ArcherDefendZone->SetupAttachment(RootComponent);
	ArcherDefendZone->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ArcherDefendZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	ArcherDefendZone->SetMobility(EComponentMobility::Type::Static);
	ArcherDefendZone->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	ArcherDefendZone->SetGenerateOverlapEvents(true);
}

UE::Math::TBox<double> AWall::GetArcherDefendBox() const
{
	return ArcherDefendZone->Bounds.GetBox();
}

UE::Math::TBox<double> AWall::GetMeleeDefendBox() const
{
	return MeleeDefendZone->Bounds.GetBox();
}

void AWall::BeginPlay()
{
	Super::BeginPlay();
	
	const float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), GameMode->WorldOriginNormal);
	if (Angle > 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("flip"));
		SetActorScale3D(FVector(1,-1,1));
		//RootComponent->AddLocalRotation(FRotator(0, 0, 180), false, nullptr, ETeleportType::TeleportPhysics);
	}
}
