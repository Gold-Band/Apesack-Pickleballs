// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcBase.h"
#include "NpcFriendly.generated.h"

class UHTNComponent;
class UWidgetComponent;
class AWall;

USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> RankIcon;
};

UCLASS()
class APESACK_PICKLEBALLS_API ANpcFriendly : public ANpcBase
{
	GENERATED_BODY()

public:
	ANpcFriendly();
	
	FString GetCharacterName() const {return CharacterName;}

	const FClassInfo* GetClassInfo() const;

	const FToolInfo* GetCharacterToolInfo() const;

	static const FToolInfo* GetToolInfo(const FDataTableRowHandle& ToolHandle);
	
	void ForceTask(const TSoftObjectPtr<UTask> Task) const;
	
	int GetKillCount() const {return KillCount;}

	void OnExternalWallChanged(AWall* Wall);

	AWall* GetGuardingWall() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	virtual void PostInitializeComponents() override;
private:
	UPROPERTY()
	TObjectPtr<AWall> GuardingWall;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UTask> GotoWallTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString CharacterName;
	
	int KillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> NameTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Default")
	FDataTableRowHandle CharacterClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Default")
	FDataTableRowHandle CharacterTool;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> ToolSpriteComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHTNComponent> HtnDomain;
};