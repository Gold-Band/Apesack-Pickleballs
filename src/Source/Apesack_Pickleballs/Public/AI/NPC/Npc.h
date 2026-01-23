// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/ClickableActor.h"
#include "AI/HTN/Task.h"

#include "AI/Actions/MeleeAttack.h"
#include "AI/Actions/MoveTimed.h"
#include "AI/Actions/MoveTo.h"
#include "AI/Actions/TargetNearestEnemy.h"
#include "AI/Actions/TargetPlayer.h"
#include "AI/Actions/Cooldown.h"

#include "GameplayTagContainer.h"
#include "AI/Actions/RangedAttack.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"

#include "Npc.generated.h"

class UHTNComponent;
class UStatsComponent;
class UNpcManager;
class UCircularPawnMovementComponent;

class UPaperSprite;
class UPaperSpriteComponent;

USTRUCT(BlueprintType)
struct FToolInfo : public FTableRowBase // row name is tool name
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ToolTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle NextTool;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ToolIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperSprite> ToolSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ToolDamage = 1;
};


USTRUCT(BlueprintType)
struct FClassInfo : public FTableRowBase // row name is class name
{
	GENERATED_BODY()
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<TSoftObjectPtr<class UTaskAsset>> ClassTasks;

	// not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle BaseTool;
};


UCLASS()
class APESACK_PICKLEBALLS_API ANpc : public APawn, public IClickableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpc();
	
	UFUNCTION(BlueprintPure)
	float GetCharacterPreferredRadius() const;
	
	UFUNCTION(BlueprintPure)
	float GetDirectionToTown();
	
	UStatsComponent* GetStats();
	FString GetCharacterName() const {return CharacterName;}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetProjectileSpawnLocation() const;
private:
	FVector GetProjectileSpawnLocation_Implementation() const;
public:
	
	//*
	//* Actions
	//*
	void MoveForwardScaled(float Scale);
	
	
protected:
	virtual void BeginPlay() override;

private:
	float Radius = 0;
	
	// direction to the player's town (left or right)
	float OriginDirection = 0;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString CharacterName;
	
	UPROPERTY()
	TObjectPtr<UNpcManager> NpcManager = nullptr;
	

	//**
	//** My Tasks
	//**
	FTask WanderTask{"Wander"};
	FTask FollowTask{"Follow"};
	
	FTask MeleeAttackTask{"Melee Attack"};
	FTask RangedAttackTask{"Ranged Attack"};
	FTask BuildTask{"Build/Repair"};
	FTask HideTask{"Hide"};
	FTask ManTowerTask{"Man Archer Tower"};
	FTask DefendWallTask{"Defend Wall"};
	
	
	//**
	//** My Components
	//**
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCircularPawnMovementComponent> MovementComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY()
	TObjectPtr<UStatsComponent> Stats = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHTNComponent> HtnDomain = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComp = nullptr;

	
	// Actions
public:
	
	//*
	//* Move
	//*
	FMoveTimedAction MoveTimedAction{this};
	FMoveToAction MoveToAction{this};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderSpeed = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMin = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move Timed")
	float WanderTimeMax = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StopDistance = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float StartRaycastingDistanceSquared = 50000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Move To")
	float RaycastInterval = 0.2f;
	
	//*
	//* Targeting
	//*
	FTargetPlayerAction TargetPlayerAction{this};
	FTargetNearestEnemyAction TargetNearestEnemyAction{this};
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Targeting")
	TObjectPtr<AActor> TargetActor = nullptr;
	
	//*
	//* Attack
	//*
	FMeleeAttackAction MeleeAttackAction{this};
	FRangedAttackAction RangedAttackAction{this};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack")
	float Cooldown_MeleeAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Melee Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_MeleeAttack = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack")
	float Cooldown_RangedAttack = 0.75f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action Properties|Ranged Attack", meta=(DisplayName="Base Damage"))
	float BaseDamage_RangedAttack = 1;
	
	//*
	//* Delay
	//*
	FCooldownAction CooldownAction{this};
	float Delay;
	
	//*
	//* Build
	//*
	
	
	//*
	//* Upgrades?
	//*
	
	
	//*
	//* Survival?
	//*

};
