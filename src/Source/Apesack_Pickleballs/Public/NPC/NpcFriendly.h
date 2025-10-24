// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcBase.h"
#include "NpcFriendly.generated.h"

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

	int GetKillCount() const {return KillCount;}

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FString CharacterName;
	
	int KillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> NameTag;
};