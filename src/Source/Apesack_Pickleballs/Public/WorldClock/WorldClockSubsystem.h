// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldClockSubsystem.generated.h"


// A lightweight version of FTimespan
USTRUCT(BlueprintType)
struct FTimestamp
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamp");
	uint8 Day = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamp");
	uint8 Hour = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamp");
	uint8 Minute = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamp");
	uint8 Second = 0;
	
	FString ToString() const
    {
		return FString::Printf(TEXT("%02d:%02d:%02d:%02d"), Day, Hour, Minute, Second);
    }
};


UENUM(BlueprintType)
enum class EWorldClockBroadcastTiming : uint8
{
	EverySecond,
	EveryMinute,
	EveryHour,
	EveryDay
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeTickedSignature, const FTimestamp&, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightEndedSignature);


/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UWorldClockSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:

	static bool IsDaytime;
	
	static UWorldClockSubsystem* Get(const UObject* WorldContext)
    {
        if (const UWorld* W = WorldContext ? WorldContext->GetWorld() : nullptr)
            return W->GetSubsystem<UWorldClockSubsystem>();
        return nullptr;
    }

	virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldClockSubsystem, STATGROUP_Tickables);
    }

    // Tell UE this subsystem ticks automatically
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override
    {
        return true; // always create one per world
    }

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void Pause();
	
	UFUNCTION(BlueprintCallable)
	void Play();

	UPROPERTY(BlueprintAssignable)
	FOnTimeTickedSignature OnTimeTickedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnNightStartedSignature OnNightStartedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnNightEndedSignature OnNightEndedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SetTimeScale(float NewTimeScale);
	
	UFUNCTION(BlueprintPure)
	float GetTimeScale() const;

	UFUNCTION(BlueprintCallable)
	void AllowClockTicking(bool AllowTicking);

	UFUNCTION(BlueprintPure)
	bool IsTickingEnabled() const;
	
	UFUNCTION(BlueprintCallable)
	void SetTime(const uint8 NewDay, const uint8 NewHour, const uint8 NewMinute, const uint8 NewSecond);
	
	UFUNCTION(BlueprintPure)
	FTimestamp GetTime() const;

	UFUNCTION(BlueprintPure)
	uint8 GetDays() const;

	UFUNCTION(BlueprintPure)
	uint8 GetHours() const;

	UFUNCTION(BlueprintPure)
	uint8 GetMinutes() const;

	UFUNCTION(BlueprintPure)
	uint8 GetSeconds() const;

private:
	uint32 Day = 0;
	uint32 Hour = 18;
	uint32 Minute = 0;
	uint32 Second = 0;
	FTimestamp CurrentTime;

	uint8 DayHourStart = 6;
	uint8 NightHourStart = 22;
	
	bool bAllowClockTicking = true;
	bool bIsNight = false;
	
	float TimeScale = 1000;

	float Milliseconds = 0;
	int msFloor = 0;

	EWorldClockBroadcastTiming BroadcastTiming = EWorldClockBroadcastTiming::EverySecond;

private:
	void TryBroadcast(EWorldClockBroadcastTiming TimingType);
	virtual void Deinitialize() override;
};
