// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcFriendly.h"
#include "Components/WidgetComponent.h"
#include "NPC/NpcName.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	NameTag = CreateDefaultSubobject<UWidgetComponent>("Name");
	NameTag->SetupAttachment(RootComponent);
	
	if (CharacterName.IsEmpty())
	{
		// get random name
		static ConstructorHelpers::FObjectFinder<UDataTable> NamesDataTableFinder(TEXT("/Game/NPC/NpcNames.NpcNames"));
		if (NamesDataTableFinder.Succeeded())
		{
			UDataTable* NamesDataTable = NamesDataTableFinder.Object;
			TArray<FNpcName*> AllNames;
			NamesDataTable->GetAllRows(TEXT("GetRandomName"), AllNames);
			CharacterName = *AllNames[FMath::RandRange(0, AllNames.Num() - 1)]->SampleName;
		}
	}
}
