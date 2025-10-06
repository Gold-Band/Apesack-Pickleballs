// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridNode.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGridNode : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSelected();
	
private:
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> Icon;
};
