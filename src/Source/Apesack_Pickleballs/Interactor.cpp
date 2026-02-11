#include "Interactor.h"
#include "Engine/LevelScriptActor.h"

UInteractor::UInteractor() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractor::BeginInteractOverlap(AActor* OtherActor) {
	UInteractable* Interactable = IsInteractable(OtherActor);
	if(!Interactable) return;
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		1,
		FColor::Emerald,
		TEXT("Found")
	);*/
	CurrentInteractable = Interactable;
}

void UInteractor::EndInteractOverlap(AActor* OtherActor) {
	// if the other actor exists and it's the same as our current interactable
	if(OtherActor && CurrentInteractable == IsInteractable(OtherActor)) {
		CurrentInteractable = nullptr;
	}
}

void UInteractor::Interact()
{
	if (!CurrentInteractable) return;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UWorld* World = GetWorld();
	if (!World) return;

	ALevelScriptActor* LevelScript = World->GetLevelScriptActor();
	if (!LevelScript) return;

	UFunction* Func = LevelScript->FindFunction(TEXT("OnInteractorUsed"));
	if (!Func) return;

	struct
	{
		AActor* OwnerActor;
	} Params;

	Params.OwnerActor = OwnerActor;

	LevelScript->ProcessEvent(Func, &Params);

	// keep your existing logic
	CurrentInteractable->Interact(OwnerActor);
}


int UInteractor::RequestedCoins() {
	if (!CurrentInteractable) return 0;
	return CurrentInteractable->Cost;
}


UInteractable* UInteractor::IsInteractable(const AActor* Actor) {
	if(!Actor) return nullptr;
	UInteractable* Interactable = Cast<UInteractable>(Actor->FindComponentByClass<UInteractable>());
	return Interactable;
}
