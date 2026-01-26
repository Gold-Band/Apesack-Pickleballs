// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Arrow.generated.h"

UCLASS()
class APESACK_PICKLEBALLS_API AArrow : public AProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArrow();

	virtual void Tick(float DeltaTime) override;
	
	float Damage = 0;	
};
