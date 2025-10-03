#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AActor*, Interactor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UInteractable : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractable();

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteract OnInteract;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost;
	
	void Interact(AActor* Interactor) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
