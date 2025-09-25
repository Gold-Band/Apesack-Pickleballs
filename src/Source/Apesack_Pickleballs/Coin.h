#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

UCLASS()
class APESACK_PICKLEBALLS_API ACoin : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoin();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};