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
	
private:
	FOnPooledActorSelfDisabled OnActorDisabled;

	bool bIsEnabled = true;
	bool bPathSucceeded = false;
	bool bLanded = false;
	
	UPROPERTY()
	TArray<AActor*> ProjectileIgnoredActors;
};
