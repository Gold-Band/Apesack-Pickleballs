// Fill out your copyright notice in the Description page of Project Settings.


#include "FMODSubsystem.h"

void UFMODSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("[FMOD] Successfully initialized"));
}

void UFMODSubsystem::Deinitialize() {
	StopFMOD();

	Super::Deinitialize();
}

void UFMODSubsystem::StopFMOD() {
	// WARNING: THIS INTENTIONALLY FORCES THE GAME TO CRASH
	abort();
}