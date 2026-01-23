// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/Projectile.h"
#include "Components/BoxComponent.h"

// Sets default values
AProjectile::AProjectile()
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
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::Launch(const FVector& Direction, float Force)
{
	Collider->AddImpulse(Direction * Force);
}

void AProjectile::LaunchAt_Implementation(const FVector& StartLocation, const FVector& TargetLocation, float ArcParam, float Accuracy)
{
	
}

FOnPooledActorSelfDisabled& AProjectile::GetOnActorDisabled()
{
	return OnActorDisabled;
}

void AProjectile::Disable()
{
	Collider->SetSimulatePhysics(false);
	Collider->SetEnableGravity(false);
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetLifeSpan(0.f);
	bIsEnabled = false;

	if (OnActorDisabled.IsBound()) OnActorDisabled.Broadcast(this);
}

void AProjectile::Enable()
{
	Collider->SetSimulatePhysics(true);
	Collider->SetEnableGravity(true);
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsEnabled = true;
}