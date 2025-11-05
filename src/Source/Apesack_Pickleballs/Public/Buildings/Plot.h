#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Plot.generated.h"

class UPaperSpriteComponent;
class UBoxComponent;

USTRUCT(BlueprintType)
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
};

UCLASS()
class APESACK_PICKLEBALLS_API APlot : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PlotTag;

	const FBuildingInfo* Building;

	void SetBuilding(const TSubclassOf<AActor> BuildingActorClass, const FBuildingInfo* BuildingInfo);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> BuildingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> BoxCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComp;
};
