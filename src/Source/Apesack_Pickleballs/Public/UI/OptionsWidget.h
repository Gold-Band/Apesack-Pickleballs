// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class UGridNode;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SelectNext();
	void ConfirmSelection();
	
private:
	//virtual bool Initialize() override;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UVerticalBox> Options;

	UPROPERTY()
	TArray<UGridNode*> OptionNodes;
	 
	uint8 SelectedOptionIndex = 0;
};
