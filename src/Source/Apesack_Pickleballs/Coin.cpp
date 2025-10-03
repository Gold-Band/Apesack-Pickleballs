#include "Coin.h"

ACoin::ACoin() {
	PrimaryActorTick.bCanEverTick = true;
}

void ACoin::BeginPlay() {
	Super::BeginPlay();
	FRotator RandomRotation = FRotator(
		0,
		FMath::RandRange(0.0f, 360.0f), // Random Yaw
		0
	);
	SetActorRotation(RandomRotation);	
}

void ACoin::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotationSpeed * DeltaTime; // Rotate around the Yaw axis
	SetActorRotation(NewRotation);
}