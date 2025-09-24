// Fill out your copyright notice in the Description page of Project Settings.


#include "GoapDisplayer.h"
#include "GoapAgent.h"
#include "GoapDisplay.h"


UGoapDisplayer::UGoapDisplayer()
{
	SetManuallyRedraw(true);
	SetDrawAtDesiredSize(true);
	SetPivot(FVector2D(0.5f, 1.0f));
	SetTickMode(ETickMode::Automatic);
	SetRedrawTime(0.25f);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.25f;
    
}


void UGoapDisplayer::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//UE_LOG(LogTemp, Warning, TEXT("UGoapDisplayer::TickComponent"));	
	if (DisplayWidget && Agent)
	{
		DisplayWidget->UpdateContent(Agent->CurrentState, Agent->Goals, Agent->Actions);
	}
}

void UGoapDisplayer::BeginPlay()
{
	Super::BeginPlay();

	// cache the widget
	DisplayWidget = Cast<UGoapDisplay>(GetWidget());

	if (!DisplayWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Widget is not a UGoapDisplay!"));
	}

	// cache the agent
	Agent = Cast<UGoapAgent>(GetOwner()->GetComponentByClass(UGoapAgent::StaticClass()));
	if (!Agent)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find agent component!"));
	}
}

