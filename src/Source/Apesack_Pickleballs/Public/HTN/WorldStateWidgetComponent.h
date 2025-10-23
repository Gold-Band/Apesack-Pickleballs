// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WorldStateWidgetComponent.generated.h"


class UHTNComponent;
struct FWorldState;
class UWorldStateWidget;
/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APESACK_PICKLEBALLS_API UWorldStateWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

	UWorldStateWidgetComponent();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UHTNComponent> Domain;

	UPROPERTY()
	TObjectPtr<UWorldStateWidget> DisplayWidget;
};
