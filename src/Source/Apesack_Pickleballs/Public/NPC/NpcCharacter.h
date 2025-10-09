// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcManager.h"
#include "GameFramework/Pawn.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "NpcCharacter.generated.h"

struct FRankInfo;
class UOptionsWidget;
class UInteractable;
class UWidgetComponent;
class UBoxComponent;
class UPaperSpriteComponent;
class UFloatingPawnMovement;
class UHTNComponent;


struct FNPCDescriptor
{

	FString Name;
	TObjectPtr<UTexture2D> RankIcon;
	TObjectPtr<UTexture2D> ClassIcon;
	TArray<TSoftObjectPtr<UTask>> ClassTasks;
	int KillCount;
	// tool
	// outfit
};

UCLASS()
class APESACK_PICKLEBALLS_API ANpcCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpcCharacter();

	const FClassInfo* GetClassInfo() const;
	//FRankInfo GetRankInfo() const;
	const FToolInfo* GetTool() const;
	
	FString GetCharacterName() const {return CharacterName;}

	int GetKillCount() const {return KillCount;}

	void ForceTask(const TSoftObjectPtr<UTask> Task);
	
protected:
	virtual void PostInitializeComponents() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FDataTableRowHandle CharacterClass;
	
	FDataTableRowHandle CharacterTool;
	
	int KillCount;
	
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
};