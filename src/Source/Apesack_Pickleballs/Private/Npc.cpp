// Fill out your copyright notice in the Description page of Project Settings.


#include "Npc.h"

#include "GoapAgent.h"

// Sets default values
ANpc::ANpc()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GoapAgent = CreateDefaultSubobject<UGoapAgent>(TEXT("GoapAgent"));
}

// Called when the game starts or when spawned
void ANpc::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANpc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
