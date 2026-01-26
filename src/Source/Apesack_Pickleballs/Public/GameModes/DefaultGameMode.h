// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "GameFramework/GameModeBase.h"
#include "WorldClock/WorldClockSubsystem.h"
#include "DefaultGameMode.generated.h"

struct FTimestamp;
class UNpcManager;
class UBuildingsManager;
class AProjectile;
class UNpcDelegates;
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
	
	const FVector WorldOriginNormal = FVector(0.0f, 1.0f, 0.0f);
	
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
	
private:
	void InitializeLocalBuildingsManagerReference();
	void InitializeLocalNpcManagerReference();
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> NpcNames;


	// cached manager instances
	UPROPERTY()
	TObjectPtr<UBuildingsManager> BuildingsManager;
	
	UPROPERTY()
	TObjectPtr<UNpcManager> NpcManager;
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClock;

	TActorPool<AActor> ArrowPool;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActor> ArrowClass;
};
