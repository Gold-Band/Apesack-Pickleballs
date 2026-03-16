// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "RitualZone.generated.h"

class UBoxComponent;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerOverlapStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerOverlapEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawningCompleteSignature);

UCLASS()
class APESACK_PICKLEBALLS_API ARitualZone : public ABuilding
{
	GENERATED_BODY()
	
public:
	ARitualZone();
	
	int GetNumOccupants() const {return OccupantCount;}
	void AddOccupant(AActor* NewOccupant);
	void RemoveOccupant();
	bool HasRoom() const;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta=(DisplayName="On Player Overlap Started"))
	FOnPlayerOverlapStartedSignature OnPlayerOverlapStartedDelegate;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta=(DisplayName="On Player Overlap Ended"))
	FOnPlayerOverlapEndedSignature OnPlayerOverlapEndedDelegate;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta=(DisplayName="On Spawning Complete"))
	FOnSpawningCompleteSignature OnSpawningCompleteDelegate;
	
protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDayTicked(uint8 Day);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> OverlapArea = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxOccupants = 1;
	UPROPERTY(VisibleAnywhere)
	int OccupantCount = 0;
};
