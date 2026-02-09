// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/Task.h"
#include "NpcHostile.generated.h"

class UNpcManager;

UCLASS()
class APESACK_PICKLEBALLS_API ANpcHostile : public ANpc
{
	GENERATED_BODY()

public:
	ANpcHostile();
	
	virtual TArray<UListItemObject*> GetInfo() const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void OnMeleeAttack();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual bool GetSideCheckCondition() override;
	
	virtual void BindActions() override;
	virtual void CreateBehaviours() override;
	
private:
	void OnNearestAttackableChanged(AActor* NewTarget, EOriginSide Side);
	float GetAngleBetweenVectors(const FVector& A, const FVector& B);
	
protected:
	TArray<AActor*> IgnoreActors;
	
	//**
	//** My Tasks
	//**
	FTask MeleeAttackTask{"Melee Attack"};
	FTask MoveForwardTask{"Move Forward"};
	
	
	//**
	//** My Actions
	//**
	
	//* Knockback *//
	FAction KnockbackAction{FString("Knockback")};
	void Knockback(float DeltaTime);
	bool KnockbackCondition() const;
	
	//* Walk *//
	FAction WalkAction{FString("Walk")};
	void Walk(float DeltaTime);
	
	//* Delay *//
	FAction CooldownAction{FString("Cooldown")};
	void Cooldown(float DeltaTime);
	void CooldownReset();
	float Delay;
	
	
	//* Move To *//
	FAction MoveToAction{FString("Move To")};
	void MoveTo(float DeltaTime);
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
	FAction MeleeAttackAction{FString("Attack")};
	void MeleeAttack(float DeltaTime);
	bool MeleeAttackCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack")
	float Cooldown_MeleeAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	
	//* Target Attackable *//
	FAction TargetAttackableAction{FString("Target Attackable")};
	void TargetAttackable(float DeltaTime);
	bool TargetAttackableCondition() const;
	
	
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