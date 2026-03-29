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
		const FVector CurrentPos = GetActorLocation();
		FlightTime += DeltaTime;
		Velocity += FVector(0.f, 0.f, 0.5f * -9.8f * FMath::Square(FlightTime));
		FVector NextPos = Start + Velocity * FlightTime ;
		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{EObjectTypeQuery::ObjectTypeQuery7}; // only collide with NpcHostile objects
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),CurrentPos, NextPos, ObjectTypes, false, TArray<AActor*>{}, EDrawDebugTrace::None,Hit,true))
		{
			// hit - set next pos			
			NextPos = Hit.ImpactPoint;
			bLanded = true;
			
			// try to damage what we hit
			UStatsComponent* DamageReceiver = Cast<UStatsComponent>(Hit.GetActor()->GetComponentByClass<UStatsComponent>());
			if (DamageReceiver != nullptr) DamageReceiver->ApplyDamagePatch(
				ShooterActor,
				Damage,
				SelfLifeStealPercent,
				BaseCritChance,
				CritMultiplier,
				TotalDamageScale,
				ProficiencyDamageType,
				RangedDamageScale,
				MeleeDamageScale,
				FireDamageScale,
				PoisonDamageScale,
				MagicDamageScale,
				FireDamage,
				PoisonDamage,
				MagicDamage,
				DebuffDuration,
				BleedDamage
			);
		}
		
		if (NextPos.Z <= 0)
		{
			NextPos.Z = 0;
			bLanded = true;
		}
			
		const FRotator NextRot = UKismetMathLibrary::FindLookAtRotation(CurrentPos, CurrentPos + (NextPos-CurrentPos));
		SetActorLocationAndRotation(NextPos, NextRot);
		
		if (bLanded)
		{
			SetActorTickEnabled(false);
			OnLanded();
		}
	}
}

bool AProjectile::CanLaunchAt(const FVector& StartLocation,
	const FVector& TargetLocation, float Accuracy)
{
	bPathSucceeded = true;
	
	bIsHighArc = IsLineOfSightToTargetBlocked(StartLocation, TargetLocation);
	const float BaseSpeed = bIsHighArc? Speed/2 : Speed;
	const float ProjectileSpeed = FMath::RandRange(BaseSpeed*0.8f, BaseSpeed*1.2f);
	UGameplayStatics::FSuggestProjectileVelocityParameters Params{GetWorld(), StartLocation, TargetLocation, ProjectileSpeed};
	Params.bDrawDebug = bDrawPathDebug;
	Params.TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;
	Params.CollisionRadius = 0;
	Params.bAcceptClosestOnNoSolutions = false;
	Params.bFavorHighArc = bIsHighArc;
	if (!UGameplayStatics::SuggestProjectileVelocity(Params,Velocity))
	{
		bPathSucceeded = false;
		return false;
	}
	
	return true;
}

bool AProjectile::IsLineOfSightToTargetBlocked(const FVector& StartLocation, const FVector& TargetLocation) const
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{EObjectTypeQuery::ObjectTypeQuery1}; 
	FHitResult Hit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),StartLocation, TargetLocation,ObjectTypes , false, TArray<AActor*>{}, EDrawDebugTrace::None,Hit,true))
	{
		return true;
	}
	return false;
}

FOnPooledActorSelfDisabled& AProjectile::GetOnActorDisabled()
{
	return OnActorDisabled;
}

void AProjectile::Disable()
{
	// reset
	
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetLifeSpan(0.f);
	bIsEnabled = false;
	Velocity = FVector::ZeroVector;
	ToggleRibbon(false);
	if (OnActorDisabled.IsBound()) OnActorDisabled.Broadcast(this);
	bIsHighArc = false;
}

void AProjectile::Enable()
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	ToggleRibbon(true);
	bIsEnabled = true;
	bLanded = false;
	FlightTime = 0;
	Start = GetActorLocation();
	//Velocity = Velocity.GetSafeNormal() * Speed;
}
