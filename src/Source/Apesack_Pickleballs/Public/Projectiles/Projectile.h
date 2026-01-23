// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PoolableActor.h"
#include "GameFramework/Actor.h"
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

	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& Direction, float Force);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LaunchAt(const FVector& StartLocation, const FVector& TargetLocation, float ArcParam = 0.5, float Accuracy = 1);
	
private:
	void LaunchAt_Implementation(const FVector& StartLocation, const FVector& TargetLocation, float ArcParam = 0.5, float Accuracy = 1);
	FOnPooledActorSelfDisabled OnActorDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> Collider;

	bool bIsEnabled = true;
};
