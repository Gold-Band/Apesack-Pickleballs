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


UCLASS()
class APESACK_PICKLEBALLS_API ANpcFriendly : public ANpc
{
	GENERATED_BODY()

public:
	ANpcFriendly();
	
	virtual void Tick(float DeltaSeconds) override;

	//* Adam F's for animations
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnMeleeAttack();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnBowAttack(bool bIsFacingTarget);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void BindActions() override;
	virtual void CreateBehaviours() override;
	
	virtual void OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor) override;
	
public:
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;
	virtual void OnClicked() override;
	virtual void OnClickedAway() override;
	
private:
	UFUNCTION()
	void OnNightStarted();
	
	UFUNCTION()
	void OnNightEnded();
	
	void OnRaidDetected(EOriginSide Side);

	UPROPERTY()
	UBuildingsManager* BuildingsManager;

	void NewRadiusTween(float NewRadius = 19000 + FMath::RandRange(-100, 10));
	
protected:
	virtual void OnDeath_Implementation() override;
	virtual bool GetSideCheckCondition() override;
	void JoinParty();
	void LeaveParty();
	UFUNCTION()
	void EnterFormation(EOriginSide Side);
	UFUNCTION()
	void ExitFormation();
	
	bool IsCombatant() const;
	
	float AdditionalSpeed = 500;
	
	//**
	//** General Properties
	//**
	UPROPERTY(EditAnywhere, Category = "Character Properties")
	ECharacterType CharacterClass;
	
	
	
	//**
	//** My Actions
	//**
	
	
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
	FAction MoveToVectorAction{FString("Move To Vector")};
	FAction MoveToOffsetAction{FString("Move To Vector")};
	void MoveTo(float DeltaTime); // move to an actor
	void MoveToVector(float DeltaTime); // move to a point
	void MoveToOffset(float DeltaTime); // move to an actor with an offset
	bool MoveToCondition() const;
	void MoveToReset();
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float MoveSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StopDistance = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StartRaycastingDistanceSquared = 50000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float RaycastInterval = 0.2f;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Move To")
	bool bCanMove = true;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Move To")
	FVector TargetLocation;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Move To")
	float OffsetAngle;
	
	int PartyIndex;
	
	//* Target Player *//
	bool TargetPlayerCondition() const;
	void CopyPlayerMovement(float Direction, float Speed);
	
	bool bEnabled_FollowPlayer = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Player", meta=(DisplayName="Cooldown"))
	float Cooldown_FollowPlayer = 0.2f;
	float CooldownTimer_FollowPlayer;
	
	bool bIsPartyMember = false;
	
	//* Target Enemy *//
	FAction TargetNearestEnemyAction{FString("Target Enemy")};
	void TargetNearestEnemy(float DeltaTime);
	bool TargetNearestEnemyCondition() const;
	bool bRaid;
	float TargetingDistance;
	
	//* Target Tower *//
	FAction TargetFarthestTowerAction{FString("Target Tower")};
	void TargetFarthestTower(float DeltaTime);
	bool TargetFarthestTowerCondition() const;
	bool bEnabled_TargetTower = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Tower", meta=(DisplayName="Cooldown"))
	float Cooldown_TargetTower = 1;
	
	float CooldownTimer_TargetTower;
	
	//* Target Building *//
	FAction TargetNearestBuildingAction{FString("Target Building")};
	void TargetNearestBuilding(float DeltaTime);
	bool TargetBuildingCondition() const;
	bool bEnabled_TargetBuilding = true;
	float CooldownTimer_TargetBuilding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Buildings", meta=(DisplayName="Cooldown"))
	float Cooldown_TargetBuilding = 1;
	
	//* Melee Attack *//
	FAction MeleeAttackAction{FString("Attack")};
	void MeleeAttack(float DeltaTime);
	bool MeleeAttackCondition() const;
	void SetMeleeParams();
	
	bool bEnabled_MeleeAttack = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Cooldown"))
	float Cooldown_MeleeAttack = 0.75f;
	
	float CooldownTimer_MeleeAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Range"))
	float TargetingDistance_Melee = 200.0f;
	
	//* Ranged Attack *//
	FAction RangedAttackAction{FString("Shoot")};
	void RangedAttack(float DeltaTime);
	bool RangedAttackCondition() const;
	void SetRangedParams();
	
	bool bEnabled_RangedAttack = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Cooldown"))
	float Cooldown_RangedAttack = 0.75f;
	
	float CooldownTimer_RangedAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_RangedAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Targeting Distance"))
	float TargetingDistance_Ranged = 1000.0f;
	
	//* Occupy Tower Spot *//
	FAction OccupyTowerAction{FString("Occupy Tower")};
	void OccupyTower(float DeltaTime);
	bool OccupyTowerCondition() const;
	
	
	//* Defend Wall *//
	FAction GetDefensePositionAction{FString("Get Defense Position")};
	FAction OnAssumedDefensePositionAction{FString("On Got Defense Position")};
	void GetDefensePosition(float DeltaTime);
	bool GetDefensePositionCondition() const;
	void OnAssumedDefensePosition(float DeltaTime);
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Defend Wall")
	bool bIsNighttime = false;
	
	UPROPERTY(EditAnywhere, Category="Action Properties|Defend Wall")
	float MinDistance = 0.2f; //angle away from the wall
	
	UPROPERTY(EditAnywhere, Category="Action Properties|Defend Wall")
	float ExtraDistancePerPerson = 0.1f; //angle away from the wall
	
	bool bAssumedPosition = false;
	
	//* Goto Safezone *//
	
	
	//**
	//** My Tasks
	//**
	FTask WanderTask{"Wander"};
	FTask FollowTask{"Follow"};
	
	FTask MeleeAttackTask{"Melee Attack"};
	FTask RangedAttackTask{"Ranged Attack"};
	FTask BuildTask{"Build/Repair"};
	FTask HideTask{"Hide"};
	FTask OccupyTowerTask{"Man Archer Tower"};
	FTask DefendWallTask{"Defend Wall"};
	FTask ReturnToSafeZone{"Return To Safe Zone"};

	
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Defend Wall", meta=(DisplayName="Print Debug"))
	bool bPrintDebug_DefendWall = false;
};