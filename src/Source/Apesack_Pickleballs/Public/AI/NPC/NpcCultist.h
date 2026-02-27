// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/Task.h"
#include "NpcCultist.generated.h"

class UWorldClockSubsystem;
class UNpcManager;

UCLASS()
class APESACK_PICKLEBALLS_API ANpcCultist : public ANpc
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

	//* Running Away *//
	FTask RunawayTask{"Runaway"};
	FAction RunawayAction{"Runaway"};
	void Runaway(float DeltaTime);
	bool RunawayCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	float RunawayRadius = 500.f;
	
	
	//* Summoning *//
	FTask SummoningTask{"Summoning"};
	FAction SummoningAction{"SummonEnemies"};
	void SummonEnemies(float DeltaTime);
	bool SummonCondition();
	bool IsRitualTime();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Summon Enemies"))
	void SummonEnemiesEvent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	bool bSummonEnemies = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	bool bSummonEnabled = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	int RitualStartHour = 20; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning")
	int RitualBaseQty = 1;
	
	int RitualQty;
	
	bool bRitualStarted = false;
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClockSubsystem;
	
};