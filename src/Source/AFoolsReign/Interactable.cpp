#include "Interactable.h"

#include "Components/BoxComponent.h"

UInteractable::UInteractable() {
	PrimaryComponentTick.bCanEverTick = false;

	Cost = 0;
}

void UInteractable::Interact(AActor* Interactor) const {
	OnInteract.Broadcast(Interactor);
}

void UInteractable::BeginPlay() {
	Super::BeginPlay();
}

void UInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
