// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Zone.generated.h"

UENUM(BlueprintType)
enum EZoneType : uint8
{
	Zone_None UMETA(DisplayName = "None"),
	Zone_Archer UMETA(DisplayName = "Archer"),
	Zone_Melee UMETA(DisplayName = "Melee")
};

/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UZone : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EZoneType> ZoneType;
};
