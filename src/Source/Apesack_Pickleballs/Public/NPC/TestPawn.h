// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TestPawn.generated.h"

struct FInputActionInstance;

UCLASS()
class APESACK_PICKLEBALLS_API ATestPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATestPawn();

	UFUNCTION()
	void HandleMove(const FInputActionInstance& Instance);
	
};
