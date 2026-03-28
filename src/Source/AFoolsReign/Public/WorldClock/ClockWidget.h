// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClockWidget.generated.h"

struct FTimestamp;
class UTextBlock;
/**
 * 
 */
UCLASS()
class AFOOLSREIGN_API UClockWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeTxt;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DayTxt;
	
	UPROPERTY()
	class UWorldClockSubsystem* WorldClock;
};
