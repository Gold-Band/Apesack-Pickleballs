// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelResizer.generated.h"


class AStaticMeshActor;

UCLASS()
class APESACK_PICKLEBALLS_API ALevelResizer : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ALevelResizer();

protected:

	// scale 1 = diameter of 1m
	UPROPERTY(EditAnywhere, meta=(Units="Meters"))
	float Radius = 100;

	UPROPERTY(EditAnywhere, meta=(Units="Meters"), meta=(DisplayName="Player Offset From Cylinder Edge"))
	float PlayerOffset = 10;
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	void UpdatePositionsAndScales();
	
	UPROPERTY(EditAnywhere)
	AStaticMeshActor* GroundCylinder;

	UPROPERTY(EditAnywhere, meta=(DisplayName="BP_PlayerCharacterController"))
	AActor* Player;

	UPROPERTY(EditAnywhere)
	ACameraActor* PlayerCamera;
	
	UPROPERTY(EditAnywhere, meta=(DisplayName="BP_CameraActor's Offset under Default"))
	FVector CameraOffset;
};
