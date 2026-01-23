// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/BuildingBase.h"
#include "ArcherTower.generated.h"

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API AArcherTower : public ABuildingBase
{
	GENERATED_BODY()
	
public:
	AArcherTower();

protected:
	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> WallMesh;
	
};
