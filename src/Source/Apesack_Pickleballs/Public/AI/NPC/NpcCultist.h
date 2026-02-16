// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcHostile.h"
#include "NpcCultist.generated.h"

class UWorldClockSubsystem;
class UNpcManager;

UCLASS()
class APESACK_PICKLEBALLS_API ANpcCultist : public ANpcHostile
{
	GENERATED_BODY()

public:
	ANpcCultist();
	
	virtual void Tick(float DeltaSeconds) override;
	
private:
	float SenseFriendliesInterval = 1.5f;
	float SenseTimer;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void BindActions() override;
	virtual void CreateBehaviours() override;

	FTask RunawayTask{"Runaway"};
	FAction RunawayAction{"Runaway"};
	void Runaway(float DeltaTime);
	bool RunawayCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	float RunawayRadius = 500.f;
	
	FTask SummoningTask{"Summoning"};
	FAction SummoningAction{"SummonEnemies"};
	void SummonEnemies(float DeltaTime);
	bool SummonCondition();
	
	UWorldClockSubsystem* WorldClockSubsystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	float DetectDangerRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	int RitualStartHour = 20; 
	

};