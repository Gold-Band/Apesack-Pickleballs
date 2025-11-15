// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/Wall.h"

#include "Components/BoxComponent.h"

AWall::AWall()
{
	MeleeDefendZone = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeZone"));
	MeleeDefendZone->SetupAttachment(RootComponent);
	MeleeDefendZone->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	MeleeDefendZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	MeleeDefendZone->SetMobility(EComponentMobility::Type::Static);
	MeleeDefendZone->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	MeleeDefendZone->SetGenerateOverlapEvents(false);
	
	ArcherDefendZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ArcherZone"));
	ArcherDefendZone->SetupAttachment(RootComponent);
	ArcherDefendZone->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ArcherDefendZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	ArcherDefendZone->SetMobility(EComponentMobility::Type::Static);
	ArcherDefendZone->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	ArcherDefendZone->SetGenerateOverlapEvents(false);
}

UE::Math::TBox<double> AWall::GetArcherDefendBox() const
{
	return ArcherDefendZone->Bounds.GetBox();
}

UE::Math::TBox<double> AWall::GetMeleeDefendBox() const
{
	return MeleeDefendZone->Bounds.GetBox();
}
