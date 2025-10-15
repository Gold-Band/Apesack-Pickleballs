// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "NpcCharacter.generated.h"

class UOptionsWidget;
class UInteractable;
class UWidgetComponent;
class UBoxComponent;
class UPaperSpriteComponent;
class UFloatingPawnMovement;
class UHTNComponent;

USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> RankIcon;
};


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
	TObjectPtr<UTexture2D> ToolTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ToolDamage = 1;
};


USTRUCT(BlueprintType)
struct FClassInfo : public FTableRowBase // row name is class name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<class UTask>> ClassTasks;

	// not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle BaseTool;
};


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