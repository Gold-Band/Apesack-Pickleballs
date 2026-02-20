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
		//UE_LOG(LogTemp, Warning, TEXT("Traveling"))
		// raycast and move
		
		FVector NextPos = GetActorLocation() + Velocity * DeltaTime * FMath::RandRange(AppliedForce*0.5f, AppliedForce*2.f);
		const FVector CurrentPos = GetActorLocation();
		//const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);
		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{EObjectTypeQuery::ObjectTypeQuery7};
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),CurrentPos, NextPos, ObjectTypes, false, TArray<AActor*>{}/*ProjectileIgnoredActors*/, EDrawDebugTrace::None,Hit,true))
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
				DebuffDuration
			);
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
	
	const float ProjectileSpeed = FMath::RandRange(Speed*0.8f, Speed*1.2f);
	UGameplayStatics::FSuggestProjectileVelocityParameters Params{GetWorld(), StartLocation, TargetLocation, ProjectileSpeed};
	//ProjectileIgnoredActors = Params.ActorsToIgnore = IgnoreActors;
	//ProjectileIgnoredActors.Add(this);
	Params.bDrawDebug = bDrawPathDebug;
	Params.TraceOption = ESuggestProjVelocityTraceOption::TraceFullPath;
	Params.CollisionRadius = 0;
	Params.bAcceptClosestOnNoSolutions = false;
	Params.ResponseParam.CollisionResponse.SetAllChannels(ECR_Ignore);
	Params.ResponseParam.CollisionResponse.SetResponse(ECC_WorldStatic,ECR_Block);
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
	ToggleRibbon(false);
	if (OnActorDisabled.IsBound()) OnActorDisabled.Broadcast(this);
}

void AProjectile::Enable()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	ToggleRibbon(true);
	bIsEnabled = true;
	bLanded = false;
}