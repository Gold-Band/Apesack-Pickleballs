// Fill out your copyright notice in the Description page of Project Settings.


#include "Dude.h"

// Sets default values
ADude::ADude()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADude::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADude::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADude::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

