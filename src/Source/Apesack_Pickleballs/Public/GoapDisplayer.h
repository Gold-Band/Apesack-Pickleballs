// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GoapDisplayer.generated.h"


class UGoapDisplay;
class UGoapAgent;
/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APESACK_PICKLEBALLS_API UGoapDisplayer : public UWidgetComponent
{
	GENERATED_BODY()

	UGoapDisplayer();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UGoapAgent> Agent;

	UPROPERTY()
	TObjectPtr<UGoapDisplay> DisplayWidget;
};
