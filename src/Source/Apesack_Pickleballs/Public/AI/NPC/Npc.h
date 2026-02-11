// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/ClickableActor.h"

#include "GameplayTagContainer.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"

#include "Npc.generated.h"

class FAction;
class UHTNComponent;
class UStatsComponent;
class UNpcManager;
class UCircularPawnMovementComponent;

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Peasant,
	Fighter,
	Archer,
	Builder
};


enum class ENpcTag : uint8;
enum class EOriginSide : uint8;


UCLASS()
class APESACK_PICKLEBALLS_API ANpc : public APawn, public IClickableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpc();
	
	virtual void Tick(float DeltaSeconds) override;
	
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
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	virtual void BindActions();
	virtual void CreateBehaviours();
	virtual bool GetSideCheckCondition();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();
	
	virtual void OnDeath_Implementation();
public:
	virtual void OnClicked() override;
	virtual FString GetActorName() const override;

private:
	
	// direction to the player's town (left or right)
	float OriginDirection = 0;
	
protected:
	//*
	//* General Properties
	//*
	float GetSideInterval = 1;
	float GetSideTimer;
	
	UPROPERTY(VisibleAnywhere, Category = "Character Properties")
	float Radius = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Character Properties")
	FString CharacterName;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Targeting")
	AActor* TargetActor = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Character Properties")
	EOriginSide MainSide;
	
	UPROPERTY(VisibleAnywhere, Category = "Character Properties")
	ENpcTag NpcType;
	
	float Timer = 0;
	
	int MoveDirection = 1;
	
	TArray<FHitResult> HitResults;
	
	UPROPERTY()
	TObjectPtr<UNpcManager> NpcManager = nullptr;
	
	
	//**
	//** My Components
	//**
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCircularPawnMovementComponent> MovementComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY()
	TObjectPtr<UStatsComponent> Stats = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UHTNComponent> HtnDomain = nullptr;
};
