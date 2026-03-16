// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/HTN/Task.h"
#include "NpcCultist.generated.h"

class AWall;
class UBuildingsManager;
class ARitualZone;
class UWorldClockSubsystem;
class UNpcManager;

UCLASS()
class AFOOLSREIGN_API ANpcCultist : public ANpc
{
	GENERATED_BODY()

public:
	ANpcCultist();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool GetSideCheckCondition() override;
	virtual void CreateBehaviours() override;

	//* Occupying a ritual zone *//
	FTask OccupyRitualZoneTask{"OccupyRitualZone"};
	EActionState MoveToVector(float DeltaTime);
	EActionState SelectRitualZone(float DeltaTime);
	EActionState JoinRitualCircle(float DeltaTime);
	bool OccupyRitualZoneCondition() const;
	virtual void OnDeath_Implementation() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ARitualZone* MySpawner;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnArrivedAtRitualZone"))
	void OnArrivedAtRitualZoneEvent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float MoveSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To Vector")
	float StopDistance = 200.f;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Move To")
	bool bCanMove = true;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Move To Vector")
	FVector TargetLocation;
	
	bool bIsOccupyingRitualZone = false;
	
	
	
	//* Summoning *//
	FTask SummoningTask{"Summoning"};
	EActionState SummonEnemies(float DeltaTime);
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Properties|Summoning", meta=(DisplayName="Cooldown"))
	float Cooldown_Summoning = 1.5f;
	
	int RitualQty;
	
	bool bRitualStarted = false;
	
	void RecalculateRitualStartTime();
	
	UPROPERTY()
	TObjectPtr<UBuildingsManager> BuildingsManager;
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClockSubsystem;
	
};