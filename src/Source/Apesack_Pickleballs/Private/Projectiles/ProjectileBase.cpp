// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/ProjectileBase.h"
#include "Components/BoxComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent->Mobility = EComponentMobility::Movable;

	Collider = CreateDefaultSubobject<UBoxComponent>(FName("Collider"));
	Collider->SetupAttachment(RootComponent);
	Collider->SetSimulatePhysics(true);
	Collider->SetEnableGravity(true);
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileBase::Launch(const FVector& Direction, float Force)
{
	Collider->AddImpulse(Direction * Force);
}

FOnPooledActorSelfDisabled& AProjectileBase::GetOnActorDisabled()
{
	return OnActorDisabled;
}

void AProjectileBase::Disable()
{
	Collider->SetSimulatePhysics(false);
	Collider->SetEnableGravity(false);
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetLifeSpan(0.f);
	bIsEnabled = false;
}

void AProjectileBase::Enable()
{
	Collider->SetSimulatePhysics(true);
	Collider->SetEnableGravity(true);
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsEnabled = true;
}