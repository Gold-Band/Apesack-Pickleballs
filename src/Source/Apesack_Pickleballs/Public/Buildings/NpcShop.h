// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "GameFramework/Actor.h"
#include "NpcShop.generated.h"

UENUM(BlueprintType)
enum EShopType : uint8
{
	Builder,
	Blacksmith,
	Archer
};

UCLASS()
class APESACK_PICKLEBALLS_API ANpcShop : public ABuildingBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANpcShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EShopType> ShopType;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
