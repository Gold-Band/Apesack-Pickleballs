#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Plot.generated.h"

USTRUCT(BlueprintType)
struct FBuildingInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer CompatiblePlotTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle NextBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> BuildingMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> BuildingIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

};

UCLASS()
class APESACK_PICKLEBALLS_API APlot : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PlotTag;

	FDataTableRowHandle Building;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
