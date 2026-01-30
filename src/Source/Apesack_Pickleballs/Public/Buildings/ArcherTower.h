// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "ArcherTower.generated.h"

class ANpc;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API AArcherTower : public ABuilding
{
	GENERATED_BODY()
	
public:
	AArcherTower();
	
	virtual bool HasRoom() const;
	
	virtual void AddOccupant(ANpc* NewOccupant);
	virtual void RemoveOccupant(ANpc* OldOccupant);
	
	UFUNCTION(BlueprintCallable)
	void AddSpot(USceneComponent* NewSpot);
	
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;
	
	
protected:
	
	UPROPERTY(VisibleAnywhere)
	TArray<ANpc*> Occupants;
	
	UPROPERTY(VisibleAnywhere)
	TArray<USceneComponent*> Spots;
	
	UPROPERTY(EditAnywhere)
	float MaxOccupants = 1;
	
	UPROPERTY(VisibleAnywhere)
	float NumOccupants = 0; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> TowerMesh;
	
};
