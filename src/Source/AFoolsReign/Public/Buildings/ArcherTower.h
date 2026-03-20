// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "ArcherTower.generated.h"

class ANpcFriendly;
//class ANpc;
/**
 * 
 */
UCLASS()
class AFOOLSREIGN_API AArcherTower : public ABuilding
{
	GENERATED_BODY()
	
public:
	AArcherTower();
	
	virtual bool HasRoom() const;
	
	virtual void AddOccupant(ANpcFriendly* NewOccupant);
	virtual void RemoveOccupant(ANpcFriendly* OldOccupant);
	
	UFUNCTION(BlueprintCallable)
	void AddSpot(USceneComponent* NewSpot);
	
	virtual TArray<UListItemObject*> GetInfo() const override;
	virtual TArray<UListItemObject*> GetActions() override;
	
protected:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere)
	TArray<ANpcFriendly*> Occupants;
	
	UPROPERTY(VisibleAnywhere)
	TArray<USceneComponent*> Spots;
	
	UPROPERTY(EditAnywhere)
	float MaxOccupants = 1;
	
	UPROPERTY(VisibleAnywhere)
	float NumOccupants = 0; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> TowerMesh;
	
};
