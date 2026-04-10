// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/HTN/Task.h"
#include "NpcHostile.generated.h"

class UNpcManager;

UCLASS()
class AFOOLSREIGN_API ANpcHostile : public ANpc
{
	GENERATED_BODY()

public:
	ANpcHostile();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnMeleeAttack();
	
	virtual bool IsAttacking() const override {return bIsAttacking;}
	virtual bool IsClickableActor() const override {return false;}
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void CreateBehaviours() override;
	
private:
	void OnNearestAttackableChanged(AActor* NewTarget, EOriginSide Side);
	
	// patch for a mysterious bug
	bool bIsFirstTick = true;
	bool bIsAttacking = false;
	bool bTargetCheck = false;
	
protected:
	
	//**
	//** My Tasks
	//**
	FTask MeleeAttackTask{"Melee Attack"};
	FTask MoveForwardTask{"Move Forward"};
	
	
	//**
	//** My Actions
	//**
	
	//* Walk *//
	EActionState Walk(float DeltaTime);
	
	
	//* Move To *//
	EActionState MoveTo(float DeltaTime);
	bool MoveToCondition() const;
	void MoveToReset();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StopDistance = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StartRaycastingDistanceSquared = 50000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float RaycastInterval = 0.2f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanMove = true;
	
	float MoveToTimer = 0;
	
	
	//* Melee Attack *//
	EActionState MeleeAnimation(float DeltaTime);
	EActionState CheckHit(float DeltaTime);
	bool MeleeAttackCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="ApplyDamageDelay"))
	float DamageDelay = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack")
	float Cooldown_MeleeAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Reach"))
	float Reach = 150;
	
	//* Target Attackable *//
	EActionState TargetAttackable(float DeltaTime);
	
	
	//* Delay *//
	EActionState Delay(float DeltaTime);
	float DelayTime;
	float Timer;
	
	
	
	//*
	//* Debug
	//*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_MoveTo = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_MeleeAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Cooldown", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_Cooldown = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_TargetNearestAny = false;
};