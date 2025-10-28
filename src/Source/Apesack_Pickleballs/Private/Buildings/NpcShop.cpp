// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/NpcShop.h"

#include "GameModes/DefaultGameMode.h"

// Sets default values
ANpcShop::ANpcShop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANpcShop::BeginPlay()
{
	Super::BeginPlay();
	
	// tell the gamemode that we exist
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RegisterShop(this, ShopType);
	}
}

// Called every frame
void ANpcShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

