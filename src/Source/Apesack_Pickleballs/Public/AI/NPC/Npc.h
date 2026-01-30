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
	
	
	UPROPERTY(EditAnywhere)
	ECharacterType CharacterClass;
	
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	virtual void BindActions();
	virtual void CreateBehaviours();
	
public:
	virtual void OnClicked() override;
	virtual FString GetActorName() const override;
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;

private:
	float Radius = 0;
	
	// direction to the player's town (left or right)
	float OriginDirection = 0;
	
protected:
	//*
	//* General Properties
	//*
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperSpriteComponent> SpriteComp = nullptr;
};
