// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"


// The HTN Domain for the parent actor.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class APESACK_PICKLEBALLS_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyDamagePatch(float NormalDamage);
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetMeleeDamage(float BaseDamage) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetRangedDamage(float BaseDamage) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetHealth() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	float GetMaxHealth() const;
	
private:
	float Damage;
};