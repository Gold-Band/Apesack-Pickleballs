// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/Task.h"


#include "NpcFriendly.generated.h"


class AArcherTower;
class UBuildingsManager;

UENUM(BlueprintType)
enum EDefendSide : uint8
{
	None,
	Right,
	Left
};

UENUM(BlueprintType)
enum EClassType : uint8
{
	Class_Peasant,
	Class_Builder,
	Class_Melee,
	Class_Ranger,
	MAX UMETA(Hidden)
};

/*
USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> RankIcon;
};
*/

UCLASS()
class APESACK_PICKLEBALLS_API ANpcFriendly : public ANpc
{
	GENERATED_BODY()

public:
	ANpcFriendly();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void BindActions() override;
	virtual void CreateBehaviours() override;
	
public:
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;
	
private:
	UFUNCTION()
	void OnNightStarted();
	
	UFUNCTION()
	void OnNightEnded();
	
	void OnRaidDetected(EOriginSide Side);

	UPROPERTY()
	UBuildingsManager* BuildingsManager;
	
protected:
	//**
	//** My Actions
	//**
	
	//* Wait *//
	FAction WaitAction{FString("Wait")};
	void Wait(float DeltaTime);
	
	
	//* Delay *//
	FAction CooldownAction{FString("Cooldown")};
	void Cooldown(float DeltaTime);
	void CooldownReset();
	float Delay;
	
	
	
	
	//* Move Timed *//
	FAction MoveTimedAction{FString("Move Timed")};
	void MoveTimed(float DeltaTime);
	bool MoveTimedCondition() const;
	void MoveTimedReset();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderSpeed = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMin = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMax = 3.0f;
	
	float MoveTime = 0;
	
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
	
	UPROPERTY(VisibleAnywhere)
	bool bCanMove = true;
	
	
	
	
	
	//* Target Player *//
	FAction TargetPlayerAction{FString("Target Player")};
	void TargetPlayer(float DeltaTime);
	
	//* Target Enemy *//
	FAction TargetNearestEnemyAction{FString("Target Enemy")};
	void TargetNearestEnemy(float DeltaTime);
	bool bRaid;
	
	//* Target Tower *//
	FAction TargetFarthestTowerAction{FString("Target Tower")};
	void TargetFarthestTower(float DeltaTime);
	bool TargetFarthestTowerCondition() const;
	
	//* Melee Attack *//
	FAction MeleeAttackAction{FString("Attack")};
	void MeleeAttack(float DeltaTime);
	bool MeleeAttackCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack")
	float Cooldown_MeleeAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	//* Ranged Attack *//
	FAction RangedAttackAction{FString("Shoot")};
	void RangedAttack(float DeltaTime);
	bool RangedAttackCondition() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack")
	float Cooldown_RangedAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_RangedAttack = 1;
	
	//* Occupy Tower Spot *//
	FAction OccupyTowerAction{FString("Occupy Tower")};
	void OccupyTower(float DeltaTime);
	bool OccupyTowerCondition() const;
	
	
	
	//**
	//** My Tasks
	//**
	FTask WanderTask{"Wander"};
	FTask WaitTask{"Wait"};
	FTask FollowTask{"Follow"};
	
	FTask MeleeAttackTask{"Melee Attack"};
	FTask RangedAttackTask{"Ranged Attack"};
	FTask BuildTask{"Build/Repair"};
	FTask HideTask{"Hide"};
	FTask OccupyTowerTask{"Man Archer Tower"};
	FTask DefendWallTask{"Defend Wall"};

	
	
	//*
	//* Debug
	//*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_MoveTo = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_MoveTimed = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_MeleeAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_RangedAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Cooldown", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_Cooldown = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Enemy", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_TargetNearestEnemy = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Friendly", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_TargetNearestFriendly = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Tower", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_TargetFurthestTower = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Player", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_TargetPlayer = false;
};