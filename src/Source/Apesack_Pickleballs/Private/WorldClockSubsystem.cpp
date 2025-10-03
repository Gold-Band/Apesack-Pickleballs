// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldClockSubsystem.h"

void UWorldClockSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAllowClockTicking)
	{
		Milliseconds += DeltaTime * TimeScale;
		if (Milliseconds >= 1)
		{
			msFloor = FMath::Floor(Milliseconds);
			Second += msFloor;
			Milliseconds -= msFloor;
			TryBroadcast(EWorldClockBroadcastTiming::EverySecond);
		}

		if (Second >= 60)
		{
			Minute += FMath::Floor(Second/60);
			Second = Second%60;
			TryBroadcast(EWorldClockBroadcastTiming::EveryMinute);
		}

		if (Minute >= 60)
		{
			Hour += FMath::Floor(Minute/60);
			Minute = Minute%60;
			TryBroadcast(EWorldClockBroadcastTiming::EveryHour);
		}

		if (Hour >= 24)
		{
			Day += FMath::Floor(Hour/24);
			Hour = Hour%24;
			TryBroadcast(EWorldClockBroadcastTiming::EveryDay);
		}
	}
}

void UWorldClockSubsystem::Pause()
{
	bAllowClockTicking = false;
}

void UWorldClockSubsystem::Play()
{
	bAllowClockTicking = true;
}

void UWorldClockSubsystem::TryBroadcast(EWorldClockBroadcastTiming TimingType)
{
	if (TimingType == BroadcastTiming)
	{
		CurrentTime.Day = Day;
		CurrentTime.Hour = Hour%24;
		CurrentTime.Minute = Minute%60;
		CurrentTime.Second = Second%60;
		OnTimeTickedDelegate.Broadcast(CurrentTime);
	}
}

void UWorldClockSubsystem::SetTimeScale(float NewTimeScale)
{
    TimeScale = NewTimeScale;
}
	
float UWorldClockSubsystem::GetTimeScale() const
{
    return TimeScale;
}

void UWorldClockSubsystem::AllowClockTicking(bool AllowTicking)
{
    bAllowClockTicking = AllowTicking;
}

bool UWorldClockSubsystem::IsTickingEnabled() const
{
    return bAllowClockTicking;
}

void UWorldClockSubsystem::SetTime(const uint8 NewDay, const uint8 NewHour, const uint8 NewMinute, const uint8 NewSecond)
{
    Day = NewDay;
	Hour = NewHour;
	Minute = NewMinute;
	Second = NewSecond;
}

FTimestamp UWorldClockSubsystem::GetTime() const
{
    return CurrentTime;
}

uint8 UWorldClockSubsystem::GetDays() const 
{
    return Day;
}

uint8 UWorldClockSubsystem::GetHours() const 
{
    return Hour;
}

uint8 UWorldClockSubsystem::GetMinutes() const
{
    return Minute;
}

uint8 UWorldClockSubsystem::GetSeconds() const
{
    return Second;
}