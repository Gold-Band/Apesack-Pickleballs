// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/CircularPawnMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

void UCircularPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FVector::Zero());
	Rotator.Yaw = Rotator.Yaw + 90.f;
	Rotator.Pitch = 0;
	Rotator.Roll = 0;
	PawnOwner->SetActorRotation(Rotator);
	PawnOwner->SetActorLocation(GetActorLocation().GetClampedToSize2D(0,Radius));	
}

void UCircularPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Radius = GetActorLocation().Length();
	UE_LOG(LogTemp, Warning, TEXT("Pawn radius = %f"), Radius)
}
