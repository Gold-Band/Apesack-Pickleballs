// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class UTask;
class UImage;
class UGridNode;
class UVerticalBox;

struct FOptionsData
{
	explicit FOptionsData(const TObjectPtr<UTexture2D> OptionIcon, const int OptionCost, const TSoftObjectPtr<UTask>& OptionOrderTask): Icon(OptionIcon), Cost(OptionCost), OrderTask(OptionOrderTask) {}
	
	const TObjectPtr<UTexture2D> Icon;
	const int Cost = 0;
	const TSoftObjectPtr<UTask> OrderTask;
};


UCLASS()
class APESACK_PICKLEBALLS_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const TArray<FOptionsData>& Data);

	void Reset() const;

	UGridNode* GetNode(const int Index);

private:
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UVerticalBox> Options;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> OptionNodeClass;
	
	// Node pool
	UPROPERTY()
	TArray<TObjectPtr<UGridNode>> OptionNodes;
	 
	int SelectedOptionIndex = 0;
};
