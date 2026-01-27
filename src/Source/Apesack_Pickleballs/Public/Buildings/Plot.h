#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ClickableActor.h"
#include "Plot.generated.h"

class UBuildingsManager;
class ABuilding;
class UPaperSpriteComponent;
class UBoxComponent;

/*USTRUCT(BlueprintType)
struct FBuildingInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer CompatiblePlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle NextBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BuildingMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> BuildingIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Level = 0;
};*/


// an object that spawns a building object (wall, archer tower, ...)
UCLASS()
class APESACK_PICKLEBALLS_API APlot : public AActor, public IClickableActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlot();

	void SpawnBuilding(int IndexOfBuilding) const;
	
	virtual void OnClicked() override;
	virtual FString GetActorName() const override;
	virtual TArray<UListItemObject*> GetActions() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorClicked();
	
	UPROPERTY(EditAnywhere, Category="Plot Properties")
	TArray<TSubclassOf<ABuilding>> CompatibleBuildings;
	
private:
	// the spawned building
	UPROPERTY(VisibleAnywhere)
	mutable TObjectPtr<AActor> BuildingActor;

	// for handling mouse clicks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> BoxCollider;
	
	// the plot sprite (star - temporary)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComp;
};
