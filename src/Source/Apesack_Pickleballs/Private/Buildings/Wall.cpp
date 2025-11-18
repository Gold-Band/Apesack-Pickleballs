// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/Wall.h"

#include "Buildings/Zone.h"

AWall::AWall()
{
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
