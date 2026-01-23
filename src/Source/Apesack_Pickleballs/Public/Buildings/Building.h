#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ClickableActor.h"
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
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root;
};
