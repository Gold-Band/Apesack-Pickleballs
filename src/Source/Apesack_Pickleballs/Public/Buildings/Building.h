#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ClickableActor.h"
#include "Managers/BuildingsManager.h"
#include "Building.generated.h"

class UStatsComponent;
class ADefaultGameMode;


UCLASS(abstract)
class APESACK_PICKLEBALLS_API ABuilding : public AActor, public IClickableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuilding();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Building Properties")
	float DistanceFromOrigin;
	
	virtual void OnClicked() override;
	virtual FString GetActorName() const override final;
	int GetBuildCost() const;
	
	bool IsDamaged() const;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building Properties")
	int BuildCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building Properties")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, Category="Building Properties")
	EBuildingType BuildingType;
	
	UPROPERTY(VisibleAnywhere, Category="Building Properties")
	EOriginSide BuildingSide;
	
	UPROPERTY()
	TObjectPtr<UStatsComponent> Stats;
};
