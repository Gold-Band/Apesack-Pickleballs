// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "Wall.generated.h"

class UZone;

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API AWall : public ABuilding
{
	GENERATED_BODY()
	
public:
	AWall();
	
protected:
	virtual void BeginPlay() override;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> WallMesh;
	
};
