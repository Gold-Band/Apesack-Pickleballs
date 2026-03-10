// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/HTN/Task.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/ClickableActor.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "Npc.generated.h"

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

USTRUCT(BlueprintType)
struct FKnockbackParams
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere)
	float KnockedDistance = 100;
	UPROPERTY(EditAnywhere)
	float JumpHeight = 30;
};

UCLASS()
class APESACK_PICKLEBALLS_API ANpc : public APawn, public IClickableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpc();
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintPure)
	int GetMoveDirection() const {return MoveDirection; }
	float GetDirectionTo(const FVector& Location) const;
	
	UFUNCTION(BlueprintPure)
	float GetSpeed() const;
	
	virtual void OnClicked() override;
	virtual FString GetActorName() const override;
	void MoveForwardScaled(float Scale);
	
	UStatsComponent* GetStats();
	FString GetCharacterName() const {return CharacterName;}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetProjectileSpawnLocation() const;
private:
	FVector GetProjectileSpawnLocation_Implementation() const;
	
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	virtual void CreateBehaviours();
	virtual bool GetSideCheckCondition();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();
	
	virtual void OnDeath_Implementation();
	
	UFUNCTION()
	virtual void OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor);
	
	//*
	//* General Properties
	//*
	float GetSideInterval = 1;
	float GetSideTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Character Properties")
	FString CharacterName;
	
	UPROPERTY(VisibleAnywhere, Category="Action Properties|Targeting")
	AActor* TargetActor = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Properties")
	EOriginSide MainSide;
	
	UPROPERTY(VisibleAnywhere, Category = "Character Properties")
	ENpcTag NpcType;
	
	float Timer = 0;
	
	int MoveDirection = 1;
	bool bIsLerping;
	
	
	TArray<FHitResult> HitResults;
	
	UPROPERTY()
	TObjectPtr<UNpcManager> NpcManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Properties|Knockback")
	FKnockbackParams MeleeKnockbackParams;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Properties|Knockback")
	FKnockbackParams RangedKnockbackParams;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Properties|Knockback", meta=(DisplayName="Print Knockback Debug"))
	bool bPrintDebug_Knockback = false;
	
	bool bWasHit;
	
	
	//* Wait *//
	FTask WaitTask{"Wait"};
	EActionState Wait(float DeltaTime);
	
	
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
