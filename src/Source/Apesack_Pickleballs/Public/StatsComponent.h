// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamagedSignature, float, DamageRecieved,float, UpdatedHealth, int, DamageType, AActor*, InstigatorActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealedSignature, float, HealthRecieved,float, UpdatedHealth, AActor*, InstigatorActor);

// The HTN Domain for the parent actor.

USTRUCT(BlueprintType)
struct FDamagePatch
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Damage") float NormalDamage = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float SelfLifeStealPercent = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float BaseCritChance = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float CritMultiplier = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float TotalDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float ProficiencyDamageType = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float RangedDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MeleeDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float FireDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float PoisonDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MagicDamageScale = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float FireDamage = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float PoisonDamage = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float MagicDamage = 0;
    UPROPERTY(BlueprintReadWrite, Category="Damage") float DebuffDuration = 0;
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class APESACK_PICKLEBALLS_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();
	
UFUNCTION(BlueprintImplementableEvent, Category="Damage")
void HealPatch(float HealAmount);
	
UFUNCTION(BlueprintImplementableEvent, Category="Damage")
void ApplyDamagePatch(
	AActor* Instigator,
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
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, meta=(DisplayName="On Healed"))
	FOnHealedSignature OnHealedDelegate;
	
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