// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Npc.h"


#include "NpcFriendly.generated.h"

UENUM(BlueprintType)
enum EDefendSide : uint8
{
	None,
	Right,
	Left
};

UENUM(BlueprintType)
enum EClassType : uint8
{
	Class_Peasant,
	Class_Builder,
	Class_Melee,
	Class_Ranger,
	MAX UMETA(Hidden)
};

/*
USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> RankIcon;
};
*/

UCLASS()
class APESACK_PICKLEBALLS_API ANpcFriendly : public ANpc
{
	GENERATED_BODY()

public:
	ANpcFriendly();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
};