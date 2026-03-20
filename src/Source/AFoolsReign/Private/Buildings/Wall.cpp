// Fill out your copyright notice in the Description page of Project Settings.
#include "Buildings/Wall.h"

#include "StatsComponent.h"
#include "AI/HTN/ListItemObject.h"

AWall::AWall()
{
	PrimaryActorTick.bCanEverTick = false;
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(RootComponent);
	
	Name = FString("Wall");
	BuildingType = EBuildingType::Wall;	
}

TArray<UListItemObject*> AWall::GetActions()
{
	return Super::GetActions();
}

TArray<UListItemObject*> AWall::GetInfo() const
{
	TArray<UListItemObject*> Info{};
	
	// hp
	UListItemObject* HpInfo = NewObject<UListItemObject>();
	HpInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Hp: %i/%i"), FMath::RoundToInt(Stats->GetHealth()), FMath::RoundToInt(Stats->GetMaxHealth())));
	
	Info.Add(HpInfo);
	
	return Info;
	//return TArray<UListItemObject*>{};
}
