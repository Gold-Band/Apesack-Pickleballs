// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "NpcCharacter.generated.h"


class UOptionsWidget;
class UInteractable;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture> RankIcon;
};


USTRUCT(BlueprintType)
struct FClassInfo : public FTableRowBase // row name is class name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<class UTask>> ClassTasks;
};


struct FNPCDescriptor
{

	FString Name;
	TObjectPtr<UTexture> RankIcon;
	TObjectPtr<UTexture> ClassIcon;
	TArray<TSoftObjectPtr<class UTask>> ClassTasks;
	int KillCount;
	// tool
	// outfit
};




class UBoxComponent;
class UPaperSpriteComponent;
class UFloatingPawnMovement;
class UHTNComponent;

UCLASS()
class APESACK_PICKLEBALLS_API ANpcCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpcCharacter();

	UFUNCTION(BlueprintPure)
	bool AreOptionsVisible() const;
	
	void OpenInteractionDialogue();
	void CloseInteractionDialogue();

	UOptionsWidget* GetInteractionDialogue() const;
	
	//void GetClass() const;
	//void GetTool() const; research
	//void GetRank() const;
	//void GetName() const;
	//void GetKillCount() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FDataTableRowHandle CharacterClass;
	int KillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> BoxCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHTNComponent> HtnDomain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UFloatingPawnMovement> MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> NameTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> InteractionOptions;
	
};