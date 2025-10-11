// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/LevelResizer.h"

#include "Apesack_Pickleballs/PlayerCharacter.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
ALevelResizer::ALevelResizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = false;

	// ...
}

#if WITH_EDITOR
void ALevelResizer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the property name
	FName PropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelResizer, Radius))
	{
		UpdatePositionsAndScales();
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelResizer, PlayerOffset))
	{
		UpdatePositionsAndScales();
	}
}

void ALevelResizer::UpdatePositionsAndScales()
{
	if (GroundCylinder) GroundCylinder->SetActorRelativeScale3D(FVector(Radius*2, Radius*2, GroundCylinder->GetActorRelativeScale3D().Z));
	if (Player)
	{
		const float PlayerRadius = Radius*100 - PlayerOffset*100;
		Player->SetActorLocation(FVector(0, PlayerRadius, Player->GetActorLocation().Z));
		if (PlayerCamera) PlayerCamera->SetActorLocation(Player->GetActorLocation() + CameraOffset);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Assign player!!"))
	}
}
#endif
