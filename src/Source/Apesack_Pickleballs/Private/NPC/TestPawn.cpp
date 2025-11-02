// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/TestPawn.h"
#include "InputAction.h"

// Sets default values
ATestPawn::ATestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


void ATestPawn::HandleMove(const FInputActionInstance& Instance)
{
	FVector Value = Instance.GetValue().Get<FVector>();

	// rotate
	AddMovementInput(Value.X * GetActorForwardVector());
}
