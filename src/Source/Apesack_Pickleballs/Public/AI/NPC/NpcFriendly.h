// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "AI/HTN/Task.h"
#include "Buildings/Wall.h"
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
	
	//* Adam F's for animations
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnMeleeAttack();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnBowAttack(bool bIsFacingTarget);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void CreateBehaviours() override;
	
	virtual void OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor) override;
	
public:
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;
	virtual void OnClicked() override;
	virtual void OnClickedAway() override;
	
private:
	
	UFUNCTION()
	void OnWallBuilt(AWall* Wall, EOriginSide OriginSide);
	
	UFUNCTION()
	void OnTowerBuilt(AArcherTower* ArcherTower, EOriginSide OriginSide);
	
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
	
	//**
	//** General Properties
	//**
	UPROPERTY(EditAnywhere, Category = "Character Properties")
	ECharacterType CharacterClass;

	bool bIsClicked = false;
	
	
	//**
	//** My Actions
	//**
	
	
	//* Move Timed *//
	
	EActionState MoveTimed(float DeltaTime);
	bool MoveTimedCondition() const;
	void MoveTimedReset();
	void SetMoveTime();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderSpeed = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMin = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMax = 3.0f;
	
	float MoveTime = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed", meta=(DisplayName="Cooldown"))
	float Cooldown_Wander = 1.f;
	
	//* Move To *//
	
	EActionState MoveTo(float DeltaTime); // move to an actor
	EActionState MoveToVector(float DeltaTime); // move to a point
	EActionState MoveToOffset(float DeltaTime); // move to an actor with an offset
	bool MoveCondition() const;
	bool GotoCondition() const;
	void MoveToReset();
	void OnGotoCompleted();
	bool bGotoLocation = false;
	//float TotalDistance;
	
	
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
	
	bool bCopyMovement = false;
	bool bEnabled_FollowPlayer = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Player", meta=(DisplayName="Cooldown"))
	float Cooldown_FollowPlayer = 0.2f;
	
	bool bIsPartyMember = false;
	
	//* Target Enemy *//
	EActionState TargetNearestEnemy(float DeltaTime);
	bool TargetNearestEnemyCondition() const;
	bool bRaid;
	float TargetingDistance;
	
	//* Target Tower *//
	EActionState TargetFarthestTower(float DeltaTime);
	bool TargetFarthestTowerCondition() const;
	bool bEnabled_TargetTower = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Tower", meta=(DisplayName="Cooldown"))
	float Cooldown_TargetTower = 1;
	
	//* Target Building *//
	EActionState TargetNearestBuilding(float DeltaTime);
	bool TargetBuildingCondition() const;
	bool bEnabled_TargetBuilding = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Targeting|Buildings", meta=(DisplayName="Cooldown"))
	float Cooldown_TargetBuilding = 1;
	
	//* Melee Attack *//
	EActionState MeleeAttack(float DeltaTime);
	bool MeleeAttackCondition() const;
	void SetMeleeParams();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Cooldown"))
	float Cooldown_MeleeAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Range"))
	float TargetingDistance_Melee = 200.0f;
	
	//* Ranged Attack *//
	EActionState RangedAttack(float DeltaTime);
	bool RangedAttackCondition() const;
	void SetRangedParams();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Cooldown"))
	float Cooldown_RangedAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_RangedAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Targeting Distance"))
	float TargetingDistance_Ranged = 1000.0f;
	
	//* Occupy Tower Spot *//
	EActionState OccupyTower(float DeltaTime);
	bool OccupyTowerCondition() const;
	bool bIsOccupyingTower = false;
	
	
	//* Defend Wall *//

	EActionState GetDefensePosition(float DeltaTime);
	bool GetDefensePositionCondition() const;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Defend Wall")
	bool bIsNighttime = false;
	
	UPROPERTY(EditAnywhere, Category="Action Properties|Defend Wall")
	float MinDistance = 0.2f; //angle away from the wall
	
	UPROPERTY(EditAnywhere, Category="Action Properties|Defend Wall")
	float ExtraDistancePerPerson = 0.1f; //angle away from the wall
	
	bool bAssumedPosition = false;
	
	bool bNewBuilding = false;
	
	//* Goto Safezone *//
	
	
	//**
	//** My Tasks
	//**
	FTask WanderTask{"Wander"};
	FTask FollowTask{"Follow"};
	FTask GotoTask{"Goto"};
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
	
	void LogBool(const FString& Name, const bool Value, const bool Simple = true) const;
	FString BoolToString(const bool Value, const bool Simple = false) const {return Simple? (Value? "T":"F") : (Value?"True":"False");}
};