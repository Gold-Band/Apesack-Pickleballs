// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InfoPanel.h"

#include "AI/NPC/Npc.h"
#include "Buildings/Plot.h"
#include "Buildings/Wall.h"

void UInfoPanel::MakeActionsForObject(UObject* Object)
{
	// check type
	const UClass* Class = Object->GetClass();
	if (Class == ANpc::StaticClass())
	{
		// set class
		// upgrade class
		// join party
	}
	
	else if (Class == AWall::StaticClass())
	{
		// upgrade
	}
	
	//else if (Class == AArcherTower::StaticClass())
	
	else if (Class == APlot::StaticClass())
	{
		// place wall
		// place archer tower
		// place ...
	}
}
