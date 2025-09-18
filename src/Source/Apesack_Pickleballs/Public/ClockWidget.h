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
class APESACK_PICKLEBALLS_API UClockWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeTxt;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DayTxt;

	UFUNCTION()
	void OnWorldClockTicked(const FTimestamp& NewTime);
};
