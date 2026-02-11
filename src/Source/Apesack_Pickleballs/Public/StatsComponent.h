// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamagedSignature, float, DamageRecieved,float, UpdatedHealth, int, DamageType);

// The HTN Domain for the parent actor.

USTRUCT(BlueprintType)
struct FDamagePatch
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Damage") float NormalDamage;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float SelfLifeStealPercent;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float BaseCritChance;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float CritMultiplier;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float TotalDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float ProficiencyDamageType;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float RangedDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MeleeDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float FireDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float PoisonDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MagicDamageScale;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float FireDamage;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float PoisonDamage;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MagicDamage;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float DebuffDuration;
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class APESACK_PICKLEBALLS_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();
	

UFUNCTION(BlueprintImplementableEvent, Category="Damage")
void ApplyDamagePatch(
	float NormalDamage,
	float SelfLifeStealPercent,
	float BaseCritChance,
	float CritMultiplier,
	float TotalDamageScale,
	float ProficiencyDamageType,
	float RangedDamageScale,
	float MeleeDamageScale,
	float FireDamageScale,
	float PoisonDamageScale,
	float MagicDamageScale,
	float FireDamage,
	float PoisonDamage,
	float MagicDamage,
	float DebuffDuration
);


    UFUNCTION(BlueprintImplementableEvent, Category="Damage")
    FDamagePatch GetDamagePatch() const;

	UPROPERTY(BlueprintCallable)
	FOnDeathSignature OnDeathDelegate;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, meta=(DisplayName="On Damaged"))
	FOnDamagedSignature OnDamagedDelegate;
	
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