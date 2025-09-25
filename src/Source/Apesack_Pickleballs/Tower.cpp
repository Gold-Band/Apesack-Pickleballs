#include "Tower.h"

// Sets default values
ATower::ATower() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATower::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATower::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ATower::Interact() {
	FVector scale = GetActorScale3D();
	scale *= 1.1f;
	SetActorScale3D(scale);
}
