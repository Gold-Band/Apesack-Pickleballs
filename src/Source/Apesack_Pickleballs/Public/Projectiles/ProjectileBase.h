// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PoolableActor.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UBoxComponent;

UCLASS()
class APESACK_PICKLEBALLS_API AProjectileBase : public AActor, public IPoolableActor
{
	GENERATED_BODY()

	virtual FOnPooledActorSelfDisabled& GetOnActorDisabled() override; 

public:	
	AProjectileBase();
	
	UFUNCTION(BlueprintCallable)
	virtual void Disable() override;

	UFUNCTION(BlueprintCallable)
	virtual void Enable() override;


	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& Direction, float Force);

private:
	FOnPooledActorSelfDisabled OnActorDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> Collider;

	bool bIsEnabled = true;
};
