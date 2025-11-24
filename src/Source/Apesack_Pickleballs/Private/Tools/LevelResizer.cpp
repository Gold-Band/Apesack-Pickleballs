// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/LevelResizer.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Apesack_Pickleballs/PlayerCharacter.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ObjectSaveContext.h"

// Sets default values for this component's properties
ALevelResizer::ALevelResizer()
{
	PrimaryActorTick.bCanEverTick = false;
	FEditorDelegates::PostSaveWorldWithContext.AddUObject(this, &ALevelResizer::OnSave);
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &ALevelResizer::OnContentMoved);
	FEditorDelegates::OnMapOpened.AddUObject(this, &ALevelResizer::OnLoad);
}


void ALevelResizer::AlignNpcsToPlayerRadius()
{
	const float PlayerRadius = Radius*100 - PlayerOffset*100;
	for (AActor* Npc : AllNpcs)
	{
		//UE_LOG(LogTemp, Log, TEXT("Npc %s"), *Npc->GetActorLabel());
		const FVector PlotDirection = Npc->GetActorLocation().GetUnsafeNormal2D();
		Npc->SetActorLocationAndRotation(PlotDirection * PlayerRadius, UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, PlotDirection) - FRotator(0,90,0));
	}
}


float GetActorOffsetFromLevelRadius(const AActor* Actor, float LevelRadius)
{
	return (LevelRadius*100.f) - FVector::DistXY(FVector::ZeroVector, Actor->GetActorLocation());
}

void ALevelResizer::RegisterLevelContent(TMap<AActor*, float>& OutLevelContent)
{
	if (GEditor)
	{
		UWorld* World = GEditor->GetEditorWorldContext().World();
		if (World->WorldType == EWorldType::Editor)
		{
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (Actor && Actor->GetFolderPath() == "LevelContent")
				{
					const float ActorOffset =  GetActorOffsetFromLevelRadius(Actor, Radius);
					OutLevelContent.Add(Actor, ActorOffset);
				}
				else if (Actor && Actor->GetFolderPath() == "Plots")
				{
					AllPlots.Add(Actor);
				}
				else if (Actor && Actor->GetFolderPath() == "Npcs")
				{
					AllNpcs.Add(Actor);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No World"))
		}
	}
}

void ALevelResizer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the property name
	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelResizer, Radius))
	{
		UpdateMapSizeAndContentPlacement();
		UpdatePlayerPosition();
		UpdatePlotPositions();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelResizer, PlayerOffset))
	{
		UpdatePlayerPosition();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelResizer, PlotsOffset))
	{
		UpdatePlotPositions();
	}
}

void ALevelResizer::UpdateMapSizeAndContentPlacement()
{
	if (GroundCylinder) GroundCylinder->SetActorRelativeScale3D(FVector(Radius*2, Radius*2, GroundCylinder->GetActorRelativeScale3D().Z));

	for (auto It = LevelContentActors.CreateIterator(); It; ++It)
	{
		AActor* Actor = It.Key();
		const float ActorOffset = It.Value();
		if (!Actor || Actor->GetFolderPath() != "LevelContent")
		{
			continue;
		}
		
		FVector Dir = Actor->GetActorLocation().GetSafeNormal2D();
		FVector UpOffset = FVector::UpVector * Actor->GetActorLocation().Z;
		Actor->SetActorLocation(Dir * (Radius*100.f - ActorOffset) + UpOffset);
	}
}

void ALevelResizer::UpdatePlayerPosition()
{
	if (Player)
	{
		const float PlayerRadius = Radius*100 - PlayerOffset*100;
		Player->SetActorLocation(FVector(0, PlayerRadius, Player->GetActorLocation().Z));
		if (PlayerCamera) PlayerCamera->SetActorLocation(Player->GetActorLocation() + CameraOffset);
	}
}

void ALevelResizer::UpdatePlotPositions()
{
	const float PlotsRadius = Radius*100 - PlotsOffset*100;
	for (auto Plot : AllPlots)
	{
		FVector PlotDirection = Plot->GetActorLocation().GetUnsafeNormal2D();
		Plot->SetActorLocationAndRotation(PlotDirection * PlotsRadius, UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, PlotDirection));
	}
}

void ALevelResizer::OnLoad(const FString& Filename, bool bAsTemplate)
{
	LevelContentActors.Empty();
	RegisterLevelContent(LevelContentActors);
}

void ALevelResizer::OnSave(UWorld* World, FObjectPostSaveContext ObjectPostSaveContext)
{
	LevelContentActors.Empty();
	RegisterLevelContent(LevelContentActors);
}

void ALevelResizer::OnContentMoved(UObject* Object, FPropertyChangedEvent& Event)	
{
	AActor* Actor = Cast<AActor>(Object);
	if (Actor && LevelContentActors.Contains(Actor))
	{
		LevelContentActors[Actor] = GetActorOffsetFromLevelRadius(Actor, Radius);
	}
}


#endif
