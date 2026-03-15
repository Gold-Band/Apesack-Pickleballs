// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "GameFramework/GameModeBase.h"
#include "Managers/BuildingsManager.h"
#include "WorldClock/WorldClockSubsystem.h"
#include "DefaultGameMode.generated.h"

struct FTimestamp;
class AProjectile;
class UWorldClockSubsystem;


/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	FString GetRandomNpcName() const;

	static float GetAngleBetweenVectors(const FVector& A, const FVector& B);
	
	static float GetAngleToOrigin(const FVector& FromLocation);
	
	static EOriginSide GetActorSideFromOrigin(const AActor* Actor, float& OutAngle);
	static EOriginSide GetActorSideFromOrigin(const AActor* Actor);
	static EOriginSide GetActorSideFrom(const AActor* FromActor, const AActor* OtherActor);
	
	static FVector WorldOriginNormal;
	static float GameplayRadius;
	
	UFUNCTION(BlueprintPure)
	AActor* GetArrow();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableClock = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimestamp GameStartTime = FTimestamp(0,8,0,0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameTimeScale = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NightStartHour = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NightEndHour = 6;
	
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> NpcNames;


	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClock;

	TActorPool<AActor> ArrowPool;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActor> ArrowClass;
};
