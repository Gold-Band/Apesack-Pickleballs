// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/BuildingBase.h"
#include "Wall.generated.h"

class UZone;

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API AWall : public ABuildingBase
{
	GENERATED_BODY()
	
public:
	AWall();

	UE::Math::TBox<double> GetArcherDefendBox() const;
	UE::Math::TBox<double> GetMeleeDefendBox() const;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UZone> MeleeDefendZone;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UZone> ArcherDefendZone;
	
};
