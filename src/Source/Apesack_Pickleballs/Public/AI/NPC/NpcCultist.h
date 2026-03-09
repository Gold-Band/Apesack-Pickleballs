// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
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
	
protected:
	virtual void BeginPlay() override;
	
	virtual void CreateBehaviours() override;

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
	
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClockSubsystem;
	
};