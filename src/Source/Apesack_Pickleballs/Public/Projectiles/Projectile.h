// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PoolableActor.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Projectile.generated.h"

class UBoxComponent;

UCLASS(abstract, BlueprintType)
class APESACK_PICKLEBALLS_API AProjectile : public AActor, public IPoolableActor
{
	GENERATED_BODY()

	virtual FOnPooledActorSelfDisabled& GetOnActorDisabled() override; 

public:	
	AProjectile();
	
	UFUNCTION(BlueprintCallable)
	virtual void Disable() override;

	UFUNCTION(BlueprintCallable)
	virtual void Enable() override;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnLanded();
	
	UFUNCTION(BlueprintCallable)
	virtual bool LaunchAt(const TArray<AActor*>& IgnoreActors, const FVector& StartLocation, const FVector& TargetLocation, float Accuracy = 1);
	
	UPROPERTY(EditAnywhere)
	float Speed = 1000.0f;
	
	UPROPERTY(EditAnywhere)
	float AppliedForce = 1.5f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=PredictProjectilePathResult)
	FPredictProjectilePathResult PredictResult;
	
	UPROPERTY(VisibleAnywhere)
	FVector Velocity;
	
	UPROPERTY(VisibleAnywhere)
	AActor* ShooterActor;
	
	UPROPERTY(VisibleAnywhere)
	float TransferDamage = 1;
	
	UPROPERTY(EditAnywhere)
	bool bDrawPathDebug = false;


	// Base / core
	float Damage = 5.f;
	float TotalDamageScale = 1.f;

	// Crit
	float BaseCritChance = 0.f;
	float CritMultiplier = 1.f;

	// Sustain
	float SelfLifeStealPercent = 0.f;

	// Damage type scales
	float RangedDamageScale = 1.f;
	float MeleeDamageScale = 1.f;
	float FireDamageScale = 1.f;
	float PoisonDamageScale = 1.f;
	float MagicDamageScale = 1.f;

	// Flat elemental damage
	float FireDamage = 0.f;
	float PoisonDamage = 0.f;
	float MagicDamage = 0.f;

	// Effects
	float DebuffDuration = 0.f;

   float ProficiencyDamageType = 1.f;


	
private:
	FOnPooledActorSelfDisabled OnActorDisabled;

	bool bIsEnabled = true;
	bool bPathSucceeded = false;
	bool bLanded = false;





};
