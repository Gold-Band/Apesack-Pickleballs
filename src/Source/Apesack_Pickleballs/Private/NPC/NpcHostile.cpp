// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcHostile.h"

// Sets default values
ANpcHostile::ANpcHostile()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void ANpcHostile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	AddMovementInput(GetActorForwardVector());
}

void ANpcHostile::BeginPlay()
{
	Super::BeginPlay();


}
