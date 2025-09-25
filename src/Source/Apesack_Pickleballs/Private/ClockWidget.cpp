// Fill out your copyright notice in the Description page of Project Settings.


#include "ClockWidget.h"

#include "WorldClockSubsystem.h"
#include "Components/TextBlock.h"

void UClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TimeTxt) return;
	
	TimeTxt->SetText(FText::FromString(FString("00:00:00")));

	
	if (UWorldClockSubsystem* WorldClockSub = UWorldClockSubsystem::Get(this))
	{
		WorldClockSub->OnTimeTickedDelegate.AddUniqueDynamic(this, &ThisClass::OnWorldClockTicked);
	}
}

void UClockWidget::OnWorldClockTicked(const FTimestamp& NewTime)
{
	if (TimeTxt)
	{
		TimeTxt->SetText(FText::FromString(FString::Printf(TEXT("%02hu:%02hu:%02hu"), NewTime.Hour, NewTime.Minute, NewTime.Second)));
	}

	if (DayTxt)
	{
		DayTxt->SetText(FText::FromString(FString::Printf(TEXT("Day: %hu"), NewTime.Day)));
	}
}
