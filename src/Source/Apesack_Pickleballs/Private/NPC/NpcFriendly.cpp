// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcFriendly.h"
#include "Components/WidgetComponent.h"
#include "GameModes/DefaultGameMode.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	NameTag = CreateDefaultSubobject<UWidgetComponent>("Name");
	NameTag->SetupAttachment(RootComponent);
}

void ANpcFriendly::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterName.IsEmpty())
	{
		const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
		if (!GameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("ANpcFriendly::ANpcFriendly - Failed to get the game mode!"))		
			return ;
		}
		CharacterName = GameMode->GetRandomNpcName();
	}
}
