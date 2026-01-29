#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ClickableActor.h"
#include "Managers/BuildingsManager.h"
#include "Building.generated.h"

class ADefaultGameMode;


UCLASS(abstract)
class APESACK_PICKLEBALLS_API ABuilding : public AActor, public IClickableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuilding();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DistanceFromOrigin;
	
	virtual void OnClicked() override;
	virtual FString GetActorName() const override final;
	int GetBuildCost() const;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BuildCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root;
	
	EBuildingType BuildingType;
};
