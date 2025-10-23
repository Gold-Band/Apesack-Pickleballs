// Fill out your copyright notice in the Description page of Project Settings.

#include "HTN/WorldStateWidgetComponent.h"
#include "HTN/HTNComponent.h"
#include "HTN/WorldStateWidget.h"

UWorldStateWidgetComponent::UWorldStateWidgetComponent()
{
	SetDrawAtDesiredSize(true);
	SetPivot(FVector2D(0.5f, 1.0f));
	SetTickMode(ETickMode::Automatic);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UWorldStateWidgetComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Domain)
	{
		DisplayWidget->UpdateContent(Domain->WorldStateContainer);
	}
}

void UWorldStateWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	// cache the widget
	DisplayWidget = Cast<UWorldStateWidget>(GetWidget());
	if (!DisplayWidget) UE_LOG(LogTemp, Error, TEXT("Widget is not a UWorldStateWidget!"));

	// cache the agent
	Domain = Cast<UHTNComponent>(GetOwner()->GetComponentByClass(UHTNComponent::StaticClass()));
	if (!Domain)UE_LOG(LogTemp, Error, TEXT("Can't find the local HTN Component on %s!"), *GetOwner()->GetName());
}

