#pragma once

#include "CoreMinimal.h"
#include "InteractableLegacy.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS()
class APESACK_PICKLEBALLS_API ATower : public AActor {
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		ATower();

		UFUNCTION(BlueprintCallable)
		void Interact();

		virtual void Tick(float DeltaTime) override;

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
};
