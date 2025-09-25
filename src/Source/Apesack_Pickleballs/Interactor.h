#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Components/ActorComponent.h"
#include "Interactor.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UInteractor : public UActorComponent {
	GENERATED_BODY()

public:
	UInteractor();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInteractable> CurrentInteractable;

	UFUNCTION(BlueprintCallable)
	void BeginInteractOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void EndInteractOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	int RequestedCoins();

	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// virtual void BeginPlay() override;

	static UInteractable* IsInteractable(const AActor* Actor);
};
