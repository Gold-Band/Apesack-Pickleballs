// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/Projectile.h"

#include "StatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent->Mobility = EComponentMobility::Movable;
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bPathSucceeded)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Travelling"))
		// raycast and move
		
		FVector NextPos = GetActorLocation() + Velocity * DeltaTime * AppliedForce;
		const FVector CurrentPos = GetActorLocation();
		const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera);
		const TArray<AActor*> IgnoreActors = {this, ShooterActor};
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(),CurrentPos, NextPos, TraceChannel, false, IgnoreActors, EDrawDebugTrace::None,Hit,true))
		{
			// hit - set next pos			
			NextPos = Hit.ImpactPoint;
			bLanded = true;
			
			// try to damage what we hit
			UStatsComponent* DamageReceiver = Cast<UStatsComponent>(Hit.GetActor()->GetComponentByClass<UStatsComponent>());
			if (DamageReceiver != nullptr) DamageReceiver->ApplyDamagePatch(TransferDamage);
		}
		
		if (NextPos.Z <= 0)
		{
			NextPos.Z = 0;
			bLanded = true;
		}
			
		const FRotator NextRot = UKismetMathLibrary::FindLookAtRotation(CurrentPos, CurrentPos + (NextPos-CurrentPos));
		SetActorLocationAndRotation(NextPos, NextRot);
		// todo - Apply radius functionality
		
		
		// change velocity
		Velocity += FVector::DownVector * 9.8f * 2;
		
		
		if (bLanded)
		{
			SetActorTickEnabled(false);
			OnLanded();
		}
	}
}

bool AProjectile::LaunchAt(const TArray<AActor*>& IgnoreActors, const FVector& StartLocation,
	const FVector& TargetLocation, float Accuracy)
{
	bPathSucceeded = true;
	
	UGameplayStatics::FSuggestProjectileVelocityParameters Params{GetWorld(), StartLocation, TargetLocation, Speed};
	Params.ActorsToIgnore = IgnoreActors;
	Params.bDrawDebug = bDrawPathDebug;
	Params.TraceOption = ESuggestProjVelocityTraceOption::TraceFullPath;
	Params.CollisionRadius = 0;
	Params.bAcceptClosestOnNoSolutions = false;
	if (!UGameplayStatics::SuggestProjectileVelocity(Params,Velocity))
	{
		bPathSucceeded = false;
		return false;
	}
	
	SetActorLocation(StartLocation);
	Enable();
	return true;
}

FOnPooledActorSelfDisabled& AProjectile::GetOnActorDisabled()
{
	return OnActorDisabled;
}

void AProjectile::Disable()
{
	// reset
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetLifeSpan(0.f);
	bIsEnabled = false;
	Velocity = FVector::ZeroVector;

	if (OnActorDisabled.IsBound()) OnActorDisabled.Broadcast(this);
}

void AProjectile::Enable()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsEnabled = true;
	bLanded = false;
}